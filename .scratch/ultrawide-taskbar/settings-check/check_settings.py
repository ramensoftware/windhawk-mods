"""Build the mod and exercise local storage/menu hooks in a standalone XAML host."""
from pathlib import Path
import re
import subprocess
import tempfile
import tomllib
import sys
from PIL import Image

folder = Path(__file__).resolve().parent
source = folder.parents[2] / "mods/windows-11-better-ultrawide-taskbar.wh.cpp"
if "--source" in sys.argv:
    source = Path(sys.argv[sys.argv.index("--source") + 1]).resolve()
    folder = folder / "source-check"
    folder.mkdir(exist_ok=True)
windhawk = Path("C:/Applications/Windhawk")
flags = (windhawk / "Compiler/compile_flags.txt").read_text().splitlines()
compiler = ["rtk", "proxy", str(windhawk / "Compiler/bin/clang++.exe")]
libraries = ["-lole32", "-loleaut32", "-lruntimeobject", "-ldwmapi", "-lwindowsapp",
             "-ld3d11", "-ldxgi", "-ld3dcompiler", "-lgdi32", "-luser32", "-lkernel32",
             "-lshell32", "-luuid", "-lCoreMessaging", "-lwindowscodecs", "-lshcore", "-ldxguid",
             "-lwinhttp", "-ld2d1"]
version = re.search(r"@version\s+(\S+)", source.read_text(encoding="utf-8"))[1]


def RunCheck(args, check=True, timeout=300):
    """One settings.check.exe mode. The exe runs as the direct child so a timeout kills it rather than a proxy,
    and a hang fails the run loudly instead of blocking it forever (0.4.1: --liquid-capture-only sat 8.5 h in a
    cross-apartment release)."""
    if args[:2] == ["rtk", "proxy"]:
        args = args[2:]
    try:
        return subprocess.run(args, check=check, timeout=timeout)
    except subprocess.TimeoutExpired:
        sys.exit(f"FAIL: {Path(args[0]).name} {' '.join(args[1:])} did not exit within {timeout} s; killed as a hang")


production_flags = [flag for flag in flags if flag != "-DWH_EDITING"]
production_flags += ['-DWH_MOD_ID=L"windows-11-better-ultrawide-taskbar"',
                     f'-DWH_MOD_VERSION=L"{version}"']
if "--harness-only" not in sys.argv:
    subprocess.run([*compiler, *production_flags, "-shared", "-O2", "-Wall", "-Wextra",
                    "-Werror", str(source), "-x", "none",
                    str(windhawk / "Engine/1.7.3/64/windhawk.lib"), *libraries,
                    "-o", str(folder / "settings.build-check.dll")], check=True)
    print("PASS: production mod compiles and links with -Werror", flush=True)
    if "--build-only" in sys.argv:
        sys.exit(0)

harness = r'''
#include <cassert>
#include <csignal>
#include <cstdio>
#include <functional>
#include <dispatcherqueue.h>
#undef GetCurrentTime
#include <winrt/Windows.System.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.Provider.h>
int editorOpens = 0;
bool showPreview = false;
bool previewTrayDemo = false;
bool livePreviewData = false;
HWND borderlessWindow{};
HRESULT borderlessResult = E_FAIL;
HWND backdropWindow{};
DWORD backdropType{};
HRESULT backdropResult = E_FAIL;
HWND hostBackdropWindow{},frameMarginsWindow{};
BOOL hostBackdropEnabled{};
MARGINS frameMargins{};
HRESULT hostBackdropResult=E_FAIL,frameMarginsResult=E_FAIL;
HRESULT WINAPI TestDwmSetWindowAttribute(HWND window, DWORD attribute, LPCVOID value, DWORD size) {
    HRESULT result = DwmSetWindowAttribute(window, attribute, value, size);
    if (attribute == DWMWA_BORDER_COLOR && size == sizeof(COLORREF) &&
        *static_cast<COLORREF const*>(value) == DWMWA_COLOR_NONE) {
        borderlessWindow = window;
        borderlessResult = result;
    }
    if (attribute == DWMWA_SYSTEMBACKDROP_TYPE && size == sizeof(DWORD)) {
        backdropWindow = window;
        backdropType = *static_cast<DWORD const*>(value);
        backdropResult = result;
    }
    if (attribute == DWMWA_USE_HOSTBACKDROPBRUSH && size == sizeof(BOOL)) {
        hostBackdropWindow=window;
        hostBackdropEnabled=*static_cast<BOOL const*>(value);
        hostBackdropResult=result;
    }
    return result;
}
HRESULT WINAPI TestDwmExtendFrameIntoClientArea(HWND window,MARGINS const* margins) {
    auto result=DwmExtendFrameIntoClientArea(window,margins);
    frameMarginsWindow=window; frameMargins=*margins; frameMarginsResult=result;
    return result;
}
BOOL WINAPI TestShowWindow(HWND window, int show) {
    return ShowWindow(window, showPreview ? show : SW_HIDE);
}
HINSTANCE WINAPI TestShellExecute(HWND, LPCWSTR, LPCWSTR file, LPCWSTR args, LPCWSTR, int) {
    wchar_t system[MAX_PATH]{};
    assert(GetSystemDirectoryW(system, ARRAYSIZE(system)));
    assert(_wcsnicmp(file, system, wcslen(system)) == 0);
    assert(_wcsicmp(wcsrchr(file, L'\\') + 1, L"notepad.exe") == 0);
    assert(args && args[0] == L'"');
    ++editorOpens;
    return reinterpret_cast<HINSTANCE>(33);
}
int windhawkSettingReads = 0;
std::atomic<int> testAppsLightTheme{-1};
std::atomic<int> testClientAnimations{-1};
std::atomic<int> testHighContrast{-1};
BOOL WINAPI TestSystemParametersInfo(UINT action, UINT param, PVOID value, UINT flags) {
    if (action == SPI_GETCLIENTAREAANIMATION && testClientAnimations >= 0) {
        *static_cast<BOOL*>(value) = testClientAnimations != 0;
        return TRUE;
    }
    if (action == SPI_GETHIGHCONTRAST && testHighContrast >= 0) {
        static_cast<HIGHCONTRAST*>(value)->dwFlags = testHighContrast ? HCF_HIGHCONTRASTON : 0;
        return TRUE;
    }
    return SystemParametersInfoW(action,param,value,flags);
}
LSTATUS WINAPI TestThemeRegistry(HKEY key, LPCWSTR subkey, LPCWSTR value, DWORD flags,
                                LPDWORD type, PVOID data, LPDWORD size) {
    int light = testAppsLightTheme.load();
    if (light >= 0 && key == HKEY_CURRENT_USER && subkey && value &&
        wcscmp(subkey, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize") == 0 &&
        wcscmp(value, L"AppsUseLightTheme") == 0) {
        if (!size) return ERROR_INVALID_PARAMETER;
        DWORD available = *size;
        *size = sizeof(DWORD);
        if (type) *type = REG_DWORD;
        if (!data) return ERROR_SUCCESS;
        if (available < sizeof(DWORD)) return ERROR_MORE_DATA;
        *static_cast<DWORD*>(data) = DWORD(light);
        return ERROR_SUCCESS;
    }
    return RegGetValueW(key, subkey, value, flags, type, data, size);
}
int TestInt(PCWSTR, ...) { ++windhawkSettingReads; return 17; }
PCWSTR TestString(PCWSTR key, ...) {
    ++windhawkSettingReads;
    if (wcscmp(key, L"theme") == 0) return L"liquid";
    if (wcscmp(key, L"liquid.preset") == 0) return L"custom";
    return L"default";
}
void TestFree(PCWSTR) {}
void TestLog(PCWSTR format, ...) {
    if (!wcsstr(format, L"[UWSET]") && !wcsstr(format, L"[UWLIQUID]") && !wcsstr(format,L"[UWITEM]") && !wcsstr(format,L"[UWJUMP]")) return;
    va_list args;
    va_start(args, format);
    vfwprintf(stderr, format, args);
    va_end(args);
    fputwc(L'\n', stderr);
    fflush(stderr);
}
#undef Wh_Log
#define Wh_Log TestLog
#define Wh_GetIntSetting TestInt
#define Wh_GetStringSetting TestString
#define Wh_FreeStringSetting TestFree
#define ShellExecuteW TestShellExecute
#define ShowWindow TestShowWindow
#define RegGetValueW TestThemeRegistry
#define DwmSetWindowAttribute TestDwmSetWindowAttribute
#define DwmExtendFrameIntoClientArea TestDwmExtendFrameIntoClientArea
#define SystemParametersInfoW TestSystemParametersInfo
#include "SOURCE_PATH"
#undef SystemParametersInfoW
#undef DwmSetWindowAttribute
#undef DwmExtendFrameIntoClientArea
#undef RegGetValueW

WNDPROC testEditorOriginal{};
bool traceNativeFrame = false;
constexpr UINT kTestEditorMessage = WM_APP + 211;
struct EditorTask {
    std::function<void(SettingsEditor&)> run;
    std::exception_ptr error;
    std::atomic<bool> completed{};
};
std::atomic<int> testBackdropTicks{};
FrameworkElement TestTemplatePart(DependencyObject const& parent, PCWSTR name) {
    for (int i = 0; i < Media::VisualTreeHelper::GetChildrenCount(parent); ++i) {
        auto child = Media::VisualTreeHelper::GetChild(parent, i);
        if (auto element = child.try_as<FrameworkElement>(); element && element.Name() == name) return element;
        if (auto found = TestTemplatePart(child, name)) return found;
    }
    return nullptr;
}
LRESULT CALLBACK TestEditorProc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    if (message == WM_TIMER && wp == kEditorBackdropTimerId) ++testBackdropTicks;
    if (message == kTestEditorMessage + 1) {
        auto* ui = reinterpret_cast<SettingsEditor*>(GetWindowLongPtrW(window, GWLP_USERDATA));
        return ui->framePointerOver ? 1 : 0;
    }
    if (traceNativeFrame && (message == WM_NCPOINTERDOWN || message == WM_NCLBUTTONDOWN ||
        message == WM_NCLBUTTONDBLCLK || message == WM_SYSCOMMAND ||
        message == WM_ENTERSIZEMOVE || message == WM_EXITSIZEMOVE ||
        message == WM_NCMOUSELEAVE)) {
        fwprintf(stderr, L"NATIVE-FRAME msg=%04X wp=%llX lp=%llX\n",
            message, static_cast<unsigned long long>(wp), static_cast<unsigned long long>(lp));
        fflush(stderr);
    }
    if (message == kTestEditorMessage) {
        auto* task = reinterpret_cast<EditorTask*>(lp);
        try {
            task->run(*reinterpret_cast<SettingsEditor*>(GetWindowLongPtrW(window, GWLP_USERDATA)));
        } catch (...) { task->error = std::current_exception(); }
        task->completed = true;
        return 0;
    }
    return CallWindowProcW(testEditorOriginal, window, message, wp, lp);
}
template<class Callback> void OnEditor(Callback callback) {
    EditorTask task{callback, {}};
    SendMessageW(g_editorWindow, kTestEditorMessage, 0, reinterpret_cast<LPARAM>(&task));
    if (task.error) std::rethrow_exception(task.error);
}
void InvokeSave(SettingsEditor& editor) {
    Automation::Peers::ButtonAutomationPeer peer(editor.save);
    peer.GetPattern(Automation::Peers::PatternInterface::Invoke)
        .as<Automation::Provider::IInvokeProvider>().Invoke();
}

uint64_t PreviewPixels(std::shared_ptr<LiquidRenderer> const& renderer) {
    std::lock_guard lock(renderer->mutex);
    assert(SUCCEEDED(renderer->failure) && renderer->backBuffer);
    D3D11_TEXTURE2D_DESC desc{};
    renderer->backBuffer->GetDesc(&desc);
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = desc.MiscFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    winrt::com_ptr<ID3D11Texture2D> staging;
    winrt::check_hresult(renderer->device->CreateTexture2D(&desc, nullptr, staging.put()));
    renderer->context->CopyResource(staging.get(), renderer->backBuffer.get());
    D3D11_MAPPED_SUBRESOURCE mapped{};
    winrt::check_hresult(renderer->context->Map(staging.get(), 0, D3D11_MAP_READ, 0, &mapped));
    uint64_t hash = 14695981039346656037ull;
    bool visible = false;
    for (UINT y = 0; y < desc.Height; ++y) {
        auto* row = static_cast<uint8_t*>(mapped.pData) + y * mapped.RowPitch;
        for (UINT x = 0; x < desc.Width * 4; ++x) {
            hash = (hash ^ row[x]) * 1099511628211ull;
            if (x % 4 == 3 && row[x]) visible = true;
        }
    }
    renderer->context->Unmap(staging.get(), 0);
    assert(visible);
    return hash;
}

// The production shader's dock distance (x) and preview coverage (y) at every pixel centre of a canvas, for one
// lens, drawn into a private float target on the probe's own device with the production vertex shader. Row-major.
// optical: x is the pieces' optical field instead, the one the bezel and its normals read.
std::vector<std::array<float, 2>> DockField(LiquidRenderer::LensConstants lens, UINT width, UINT height, bool optical = false) {
    struct Probe {
        winrt::com_ptr<ID3D11Device> device;
        winrt::com_ptr<ID3D11DeviceContext> context;
        winrt::com_ptr<ID3D11VertexShader> vertex;
        winrt::com_ptr<ID3D11PixelShader> pixel, optical;
    };
    static Probe probe = [] {
        Probe p;
        if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                                     nullptr, 0, D3D11_SDK_VERSION, p.device.put(), nullptr, p.context.put())))
            winrt::check_hresult(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                                                   nullptr, 0, D3D11_SDK_VERSION, p.device.put(), nullptr, p.context.put()));
        std::string shader = LiquidRenderer::kShader;
        shader += R"(
float4 dockProbe(VertexOutput input) : SV_Target {
    float distance = dockDistance(input.position.xy + outputOffset.xy);
    return float4(distance, 1 - smoothstep(-1.5, 0.5, distance), 0, 0);
}
float4 dockOpticalProbe(VertexOutput input) : SV_Target {
    return float4(dockOutlineDistance(input.position.xy + outputOffset.xy, true), 0, 0, 0);
})";
        std::string cap = std::to_string(kMaxButtonLenses);
        D3D_SHADER_MACRO macros[]{{"BUTTON_LENS_CAP", cap.c_str()}, {nullptr, nullptr}};
        for (auto [entry, target] : {std::pair{"dockProbe", p.pixel.put()}, std::pair{"dockOpticalProbe", p.optical.put()}}) {
            winrt::com_ptr<ID3DBlob> bytecode, errors;
            HRESULT hr = D3DCompile(shader.data(), shader.size(), "dock-probe", macros, nullptr,
                entry, "ps_5_0", D3DCOMPILE_WARNINGS_ARE_ERRORS, 0, bytecode.put(), errors.put());
            if (errors) fprintf(stderr, "%s\n", static_cast<char*>(errors->GetBufferPointer()));
            winrt::check_hresult(hr);
            winrt::check_hresult(p.device->CreatePixelShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(),
                                                             nullptr, target));
        }
        auto const& vertex = LiquidRenderer::GetShaderBytecode().vertex;
        winrt::check_hresult(p.device->CreateVertexShader(vertex->GetBufferPointer(), vertex->GetBufferSize(),
                                                          nullptr, p.vertex.put()));
        return p;
    }();
    D3D11_BUFFER_DESC bufferDesc{sizeof(lens), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER};
    D3D11_SUBRESOURCE_DATA data{&lens};
    winrt::com_ptr<ID3D11Buffer> constants;
    winrt::check_hresult(probe.device->CreateBuffer(&bufferDesc, &data, constants.put()));
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = width; desc.Height = height;
    desc.MipLevels = desc.ArraySize = desc.SampleDesc.Count = 1;
    desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET;
    winrt::com_ptr<ID3D11Texture2D> texture, staging;
    winrt::com_ptr<ID3D11RenderTargetView> target;
    winrt::check_hresult(probe.device->CreateTexture2D(&desc, nullptr, texture.put()));
    winrt::check_hresult(probe.device->CreateRenderTargetView(texture.get(), nullptr, target.put()));
    desc.Usage = D3D11_USAGE_STAGING; desc.BindFlags = 0; desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    winrt::check_hresult(probe.device->CreateTexture2D(&desc, nullptr, staging.put()));
    auto rtv = target.get();
    auto buffer = constants.get();
    D3D11_VIEWPORT viewport{0, 0, float(width), float(height), 0, 1};
    probe.context->OMSetRenderTargets(1, &rtv, nullptr);
    probe.context->OMSetBlendState(nullptr, nullptr, UINT_MAX);
    probe.context->RSSetViewports(1, &viewport);
    probe.context->VSSetShader(probe.vertex.get(), nullptr, 0);
    probe.context->PSSetShader(optical ? probe.optical.get() : probe.pixel.get(), nullptr, 0);
    probe.context->PSSetConstantBuffers(0, 1, &buffer);
    probe.context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    probe.context->Draw(3, 0);
    probe.context->OMSetRenderTargets(0, nullptr, nullptr);
    probe.context->CopyResource(staging.get(), texture.get());
    D3D11_MAPPED_SUBRESOURCE mapped{};
    winrt::check_hresult(probe.context->Map(staging.get(), 0, D3D11_MAP_READ, 0, &mapped));
    std::vector<std::array<float, 2>> field(size_t(width) * height);
    for (UINT y = 0; y < height; ++y) {
        auto const* row = reinterpret_cast<float const*>(static_cast<BYTE const*>(mapped.pData) + y * mapped.RowPitch);
        for (UINT x = 0; x < width; ++x) field[size_t(y) * width + x] = {row[x * 4], row[x * 4 + 1]};
    }
    probe.context->Unmap(staging.get(), 0);
    return field;
}

// The renderer's current lens at one output pixel centre.
std::array<float, 2> DockProbe(std::shared_ptr<LiquidRenderer> const& renderer, float x, float y) {
    LiquidRenderer::LensConstants lens;
    { std::lock_guard lock(renderer->mutex); lens = renderer->lens; }
    lens.outputOffset[0] = x - .5f; lens.outputOffset[1] = y - .5f;
    return DockField(lens, 1, 1)[0];
}

std::vector<float> GeometryCoverage(ID2D1Geometry* geometry, UINT width, UINT height);
// Direct2D's antialiased fill of the outline's trace at a hull: the coverage the compositor's clip gives each pixel.
std::vector<float> TraceCoverage(DockOutline const& outline, DockHull const& hull, UINT width, UINT height) {
    return GeometryCoverage(OutlineGeometry(outline, hull).get(), width, height);
}
std::vector<float> GeometryCoverage(ID2D1Geometry* geometry, UINT width, UINT height) {
    winrt::com_ptr<IWICImagingFactory> wic;
    winrt::check_hresult(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        __uuidof(IWICImagingFactory), wic.put_void()));
    winrt::com_ptr<IWICBitmap> bitmap;
    winrt::check_hresult(wic->CreateBitmap(width, height, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, bitmap.put()));
    winrt::com_ptr<ID2D1RenderTarget> target;
    winrt::check_hresult(OutlineFactory()->CreateWicBitmapRenderTarget(bitmap.get(),
        D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_SOFTWARE,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96), target.put()));
    winrt::com_ptr<ID2D1SolidColorBrush> brush;
    winrt::check_hresult(target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), brush.put()));
    target->BeginDraw();
    target->Clear(D2D1::ColorF(0, 0.f));
    target->FillGeometry(geometry, brush.get());
    winrt::check_hresult(target->EndDraw());
    WICRect rect{0, 0, int(width), int(height)};
    winrt::com_ptr<IWICBitmapLock> lock;
    winrt::check_hresult(bitmap->Lock(&rect, WICBitmapLockRead, lock.put()));
    UINT stride{}, size{};
    BYTE* data{};
    winrt::check_hresult(lock->GetStride(&stride));
    winrt::check_hresult(lock->GetDataPointer(&size, &data));
    std::vector<float> coverage(size_t(width) * height);
    for (UINT y = 0; y < height; ++y)
        for (UINT x = 0; x < width; ++x) coverage[size_t(y) * width + x] = data[y * stride + x * 4 + 3] / 255.f;
    return coverage;
}

// One outline, two realizations: the trace Direct2D fills (what the compositor clips to) against the shader's
// piece distance (what the preview and the Liquid optics draw). A pixel centre d px inside a straight edge is
// covered 0.5 - d of the way, so the distance predicts the coverage. Pixels further than the probe's 0.4 px edge
// budget from the contour must agree exactly; the rim may differ by the probe's ~100/255.
void CheckOutlineParity(char const* name, DockOutline const& outline, DockHull const& hull, UINT width, UINT height) {
    LiquidRenderer::LensConstants lens{};
    lens.imageSize[0] = float(width); lens.imageSize[1] = float(height);
    lens.center[0] = (hull.left + hull.right) / 2; lens.center[1] = (hull.top + hull.bottom) / 2;
    lens.halfSize[0] = (hull.right - hull.left) / 2; lens.halfSize[1] = (hull.bottom - hull.top) / 2;
    lens.radius = outline.radius;
    ApplyDockShape(lens, outline, 1);
    auto field = DockField(lens, width, height);
    auto coverage = TraceCoverage(outline, hull, width, height);
    int inside = 0, outside = 0, rim = 0, mismatches = 0;
    float worst = 0;
    for (size_t i = 0; i < field.size(); ++i) {
        float d = field[i][0], c = coverage[i];
        bool agree = true;
        if (d <= -0.9f) { ++inside; agree = c == 1; }
        else if (d >= 0.9f) { ++outside; agree = c == 0; }
        else { ++rim; worst = std::max(worst, std::abs(c - std::clamp(0.5f - d, 0.f, 1.f))); }
        if (!agree && mismatches++ < 8)
            fprintf(stderr, "OUTLINE-PARITY %s pixel (%zu, %zu) distance %.3f coverage %.3f\n", name, i % width, i / width, d, c);
    }
    printf("OUTLINE-PARITY %s inside=%d outside=%d rim=%d worst=%.3f mismatches=%d\n", name, inside, outside, rim, worst, mismatches);
    assert(inside > 0 && outside > 0 && rim > 0 && mismatches == 0 && worst <= 0.4f);
}

// What slanted ends keep clear of, at the settings' defaults: button vertical padding 4 and corner radius 12 (a 44 DIP
// button's half width is more), tray highlight padding 8 and radius 12. Upright outlines ignore it. It has no icons,
// so the right end keeps the tray plate.
constexpr DockContent kContent{4, 12, 8, 12};
// The same with both icons, each centred in its band: a 44 DIP Start button's 24 DIP icon 18 DIP in from the left
// hull edge, and the bell's 16 DIP glyph 28 DIP in from the right, past 8 DIP of padding, Show desktop's 12 DIP strip
// and 8 DIP of its own item.
// `trayInset` 0 puts the glyph at the tray's very end, nearer the hull edge than Start's icon is, so the right end moves
// out.
// Separate's facing icons too: the last button's as Start's, 18 DIP in from its bar's edge, and a 16 DIP first tray
// glyph 20 DIP in from the tray bar's.
DockContent Mirrored(float band, float drop, float trayInset = 28) {
    DockContent content = kContent;
    content.buttonIcon = content.lastButtonIcon = IconBox{18, (band - 24) / 2, (band + 24) / 2};
    content.trayIcon = IconBox{trayInset, drop + (band - drop - 16) / 2, drop + (band - drop + 16) / 2};
    content.firstTrayIcon = IconBox{20, drop + (band - drop - 16) / 2, drop + (band - drop + 16) / 2};
    return content;
}

// Smaller tray's far side as the contract states it, construction (a), written out apart from the trace and the
// shader: how far inside the hull's far side it sits at x. Tall wall radius rl, tray wall radius rt = min(r, (H - d)/2),
// rho = max(rl, d/2), the S centred 6 DIP before the tray content (8 DIP padding plus the tray) and running 2 * run.
struct SmallerTrayProfile {
    float left, right, d, rl, rt, rho, run, s, c1, c2;
    SmallerTrayProfile(DockHull const& hull, float radius, float drop, float trayWidth)
        : left(hull.left), right(hull.right), d(drop), rl(radius),
          rt(std::min(radius, (hull.bottom - hull.top - drop) / 2)), rho(std::max(radius, drop / 2)),
          run(std::sqrt(4 * rho * drop - drop * drop) / 2), s(hull.right - (8 + trayWidth + 6)), c1(s - run), c2(s + run) {}
    float depth(float x) const {
        auto circle = [](float r, float u) { return std::sqrt(std::max(0.f, r * r - u * u)); };
        if (x <= c1) return 0;
        if (x <= s) return rho - circle(rho, x - c1);
        if (x <= c2) return d - rho + circle(rho, x - c2);
        if (x <= right - rt) return d;
        return d + rt - circle(rt, x - (right - rt));
    }
    float column(float x) const { // The mean depth over the pixel column from x: what an area coverage sums to.
        float sum = 0;
        for (int k = 0; k < 64; ++k) sum += depth(x + (k + .5f) / 64);
        return sum / 64;
    }
};

// The compositor's coverage of a Smaller tray trace read a column at a time: in one pixel column the uncovered run
// on the far side of the hull sums to where the far side crosses it, so each column reads the far side's depth inside
// the hull to a few hundredths of a pixel. These encode the intent; drops zeroed or a square step must fail them.
void CheckSmallerTrayCoverage(char const* name, DockStyle style, float radius, DockHull hull, bool atTop, UINT width, UINT height) {
    float band = hull.bottom - hull.top, d = float(TrayDrop(style, band));
    assert(d > 0);
    auto coverage = TraceCoverage(MakeOutline(style, 240, radius, band, atTop, kContent), hull, width, height);
    auto at = [&](float x, int y) { return coverage[size_t(y) * width + size_t(x)]; };
    // The tray band's middle row is covered in every column, even when d passes the hull's middle.
    int mid = int(atTop ? hull.top + (band - d) / 2 : hull.bottom - (band - d) / 2);
    auto farDepth = [&](float x) { // x: a column's left edge
        float sum = 0;
        if (atTop) { for (int y = mid; y < int(height); ++y) sum += at(x, y); return hull.bottom - (mid + sum); }
        for (int y = 0; y < mid; ++y) sum += 1 - at(x, y);
        return sum - hull.top;
    };
    auto edgeY = [&](float x) {
        float sum = 0;
        if (atTop) { for (int y = 0; y < mid; ++y) sum += 1 - at(x, y); return sum; }
        for (int y = mid; y < int(height); ++y) sum += at(x, y);
        return mid + sum;
    };
    SmallerTrayProfile profile(hull, radius, d, 240);
    bool separate = style.layout == DockLayout::Separate;
    float trayLeft = hull.right - 256, split = hull.right - 268;
    float worstFlat = 0, worstS = 0, worstCorner = 0;
    // Over the buttons the far side is the hull's; over the tray content it is d further in.
    float buttonsEnd = separate ? split - radius - 2 : profile.c1 - 2, trayStart = separate ? trayLeft + profile.rt + 2 : profile.c2 + 2;
    for (float x = std::ceil(hull.left + radius + 2); x + 1 <= buttonsEnd; ++x) worstFlat = std::max(worstFlat, std::abs(farDepth(x)));
    for (float x = std::ceil(trayStart); x + 1 <= hull.right - profile.rt - 2; ++x)
        worstFlat = std::max(worstFlat, std::abs(farDepth(x) - d));
    float farRow = atTop ? hull.bottom : hull.top, inward = atTop ? -1.f : 1.f;
    auto row = [&](float depth) { return int(std::floor(farRow + inward * depth - (atTop ? 1 : 0))); }; // the pixel just past depth
    float buttonColumn = std::floor(hull.left + 3 * radius), trayColumn = std::floor(hull.right - 40);
    assert(at(buttonColumn, row(1)) == 1); // Over the buttons, covered 1 px inside the far side.
    for (float depth = 0; depth < d; ++depth) assert(at(trayColumn, row(depth)) == 0); // Over the tray, uncovered for d,
    assert(at(trayColumn, row(d + 1)) == 1); // and covered after.
    if (separate) {
        for (float x = std::ceil(split) + 1; x + 1 < trayLeft; ++x) assert(at(x, mid) == 0); // The bar gap stays open.
    } else {
        // The S passes through its centre at half the drop, follows the two arcs, and is flat beyond c1 and c2.
        for (float x = std::floor(profile.c1) - 2; x <= std::ceil(profile.c2) + 2; ++x)
            worstS = std::max(worstS, std::abs(farDepth(x) - profile.column(x)));
        float centre = std::floor(profile.s);
        printf("SMALLER-TRAY %s S centre column depth %.3f (contract %.3f, half the drop %.3f)\n", name, farDepth(centre),
               profile.column(centre), d / 2);
    }
    // The tray wall's far corner is a circle of the tray band's radius, on both of the tray bar's walls with Separate.
    for (float u = 0; u <= .7f * profile.rt - 1; ++u) {
        float x = std::floor(hull.right - profile.rt + u);
        worstCorner = std::max(worstCorner, std::abs(farDepth(x) - profile.column(x)));
        if (separate) {
            float mirrored = std::floor(trayLeft + profile.rt - u) - 1, w = mirrored + .5f - trayLeft;
            worstCorner = std::max(worstCorner, std::abs(farDepth(mirrored) -
                (d + profile.rt - std::sqrt(std::max(0.f, profile.rt * profile.rt - (profile.rt - w) * (profile.rt - w))))));
        }
    }
    // The tray section's screen-edge side is flush with the button section's.
    float edgeLine = atTop ? hull.top : hull.bottom, flush = std::max(std::abs(edgeY(buttonColumn) - edgeLine),
                                                                     std::abs(edgeY(trayColumn) - edgeLine));
    printf("SMALLER-TRAY %s drop=%g tray radius=%g rho=%g run=%.3f flat=%.3f S=%.3f corner=%.3f edge=%.3f\n", name, d, profile.rt,
           profile.rho, 2 * profile.run, worstFlat, worstS, worstCorner, flush);
    // D2D's antialiasing is not exact area coverage where the S runs steeper than 45 degrees, so the S takes the parity
    // rim's budget. At d = 44 the trace's own geometry is within 0.03 px of the contract, its raster 0.34 px off by the
    // vertical tangent (0.4.5, v045-impl/debug-s.log).
    assert(worstFlat <= .05f && worstS <= .4f && worstCorner <= .15f && flush <= .02f);
}

// The trace's own signed distance in output pixels: D2D samples the outline every 0.02 DIP and tests the fill, so
// this is an oracle independent of the shader's field.
struct TraceDistance {
    winrt::com_ptr<ID2D1PathGeometry> geometry;
    std::vector<D2D1_POINT_2F> samples;
    float scale;
    TraceDistance(DockOutline const& outline, DockHull const& hull, float scale, D2D1_RECT_F region)
        : geometry(OutlineGeometry(outline, hull)), scale(scale) {
        float length{};
        winrt::check_hresult(geometry->ComputeLength(nullptr, 0.001f, &length));
        for (float l = 0; l < length; l += .02f) {
            D2D1_POINT_2F point{};
            winrt::check_hresult(geometry->ComputePointAtLength(l, nullptr, 0.001f, &point, nullptr));
            if (point.x >= region.left && point.x <= region.right && point.y >= region.top && point.y <= region.bottom)
                samples.push_back(point);
        }
    }
    float operator()(float x, float y) const {
        D2D1_POINT_2F p{x / scale, y / scale};
        float best = 1e30f;
        for (auto const& s : samples) best = std::min(best, (s.x - p.x) * (s.x - p.x) + (s.y - p.y) * (s.y - p.y));
        BOOL inside{};
        winrt::check_hresult(geometry->FillContainsPoint(p, nullptr, 0.001f, &inside));
        return (inside ? -1 : 1) * std::sqrt(best) * scale;
    }
};

// The Liquid border band, the rim and the bezel read the field inside the outline, not only its zero. At production
// scale, around the S the plain and the optical field must be the trace's own signed distance within half a pixel
// 12 DIP deep, and the border band of 1 and 10 DIP must be as wide across the S as across the flat far side.
void CheckInteriorField(char const* name, DockStyle style, float radius, bool atTop, bool bandNormals, float band = 40) {
    constexpr float scale = 1.5f, trayWidth = 120, deep = 12 * scale;
    UINT width = 660, height = 120;
    float edge = style.shape == DockShape::Island ? 4.f : 0.f, top = atTop ? edge : 80 - edge - band;
    DockHull hull{20.25f, top, 420.25f, top + band};
    auto outline = MakeOutline(style, trayWidth, radius, band, atTop, kContent);
    float d = float(TrayDrop(style, band));
    SmallerTrayProfile profile(hull, radius, d, trayWidth);
    LiquidRenderer::LensConstants lens{};
    lens.imageSize[0] = float(width); lens.imageSize[1] = float(height);
    lens.center[0] = (hull.left + hull.right) / 2 * scale; lens.center[1] = (hull.top + hull.bottom) / 2 * scale;
    lens.halfSize[0] = (hull.right - hull.left) / 2 * scale; lens.halfSize[1] = band / 2 * scale;
    lens.radius = radius * scale;
    ApplyDockShape(lens, outline, scale);
    auto field = DockField(lens, width, height), optical = DockField(lens, width, height, true);
    float x0 = (profile.c1 - 20) * scale, x1 = (profile.c2 + 20) * scale;
    TraceDistance trace(outline, hull, scale, {x0 / scale - 20, -20, x1 / scale + 20, 100});
    float worst = 0, worstOptical = 0;
    std::vector<std::pair<size_t, float>> inside; // pixel index, the trace's signed distance
    for (UINT y = 0; y < height; ++y)
        for (UINT x = UINT(x0); x < UINT(x1); ++x) {
            float t = trace(x + .5f, y + .5f);
            size_t i = size_t(y) * width + x;
            if (t >= 0) continue;
            inside.push_back({i, t});
            if (-t > deep) continue;
            worst = std::max(worst, std::abs(field[i][0] - t));
            worstOptical = std::max(worstOptical, std::abs(optical[i][0] - t));
        }
    printf("INTERIOR-FIELD %s pixels=%zu worst field error %.3f px, optical %.3f px, %g px deep\n", name, inside.size(), worst,
           worstOptical, deep);
    assert(!inside.empty() && worst <= .5f && worstOptical <= .5f);
    auto fieldAt = [&](float x, float y) {
        auto probe = lens;
        probe.outputOffset[0] = x - .5f; probe.outputOffset[1] = y - .5f;
        return DockField(probe, 1, 1)[0][0];
    };
    float farLine = atTop ? hull.bottom : hull.top, inward = atTop ? -1.f : 1.f;
    for (float w : {1.f, 10.f}) {
        float px = w * scale;
        // The shader colours a pixel by smoothstep(-w - 0.5, -w + 0.5, field): none at all deeper than w + 1 px.
        size_t deepBorder = 0;
        for (auto [i, t] : inside)
            if (-t > px + 1 && field[i][0] > -px - .5f) ++deepBorder;
        if (!bandNormals) { assert(deepBorder == 0); continue; }
        // Along the inward normal at a far-side point, the band ends where the field reaches -w.
        auto bandWidth = [&](float bx, float bv, float nx, float nv) {
            float lo = 0, hi = px + 6;
            for (int k = 0; k < 24; ++k) {
                float t = (lo + hi) / 2;
                (fieldAt((bx + t / scale * nx) * scale, (farLine + inward * (bv + t / scale * nv)) * scale) > -px ? lo : hi) = t;
            }
            return lo;
        };
        float tangent = std::atan2(d / 2 - profile.rho, profile.run), pi = 3.14159265f;
        float a1 = (-pi / 2 + tangent) / 2, a2 = (tangent + pi + pi / 2) / 2;
        float flatButtons = bandWidth(profile.c1 - 15, 0, 0, 1), flatTray = bandWidth(profile.c2 + 15, d, 0, 1);
        float convex = bandWidth(profile.c1 + profile.rho * std::cos(a1), profile.rho + profile.rho * std::sin(a1), -std::cos(a1), -std::sin(a1));
        float centre = bandWidth(profile.s, d / 2, -profile.run / profile.rho, -(d / 2 - profile.rho) / profile.rho);
        float concave = bandWidth(profile.c2 + profile.rho * std::cos(a2), d - profile.rho + profile.rho * std::sin(a2), std::cos(a2), std::sin(a2));
        printf("INTERIOR-BAND %s w=%g DIP (%.1f px): flat %.3f / %.3f, S convex %.3f centre %.3f concave %.3f px; %zu deep border pixels\n",
               name, w, px, flatButtons, flatTray, convex, centre, concave, deepBorder);
        for (float across : {flatTray, convex, centre, concave}) assert(std::abs(across - flatButtons) <= .5f);
        assert(std::abs(flatButtons - px) <= .5f && deepBorder == 0);
    }
}

void RunSmallerTrayOutline(UINT width, UINT height) {
    // TrayDrop is the one clamp: the tray band keeps 24 DIP.
    assert(TrayDrop({.trayReduction = 16}, 40) == 16 && TrayDrop({.trayReduction = 16}, 32) == 8 &&
           TrayDrop({.trayReduction = 16}, 24) == 0 && TrayDrop({.trayReduction = 8}, 20) == 0 && TrayDrop({}, 40) == 0);
    puts("PASS: TrayDrop clamps the reduction so the tray band keeps 24 DIP: band 40 drops 16, band 32 drops 8, band 24 none");
    // The S needs the whole right wall on the canvas, so these hulls end 20 DIP short of its right side.
    auto fit = [&](DockShape shape, bool atTop, float band) {
        float edge = shape == DockShape::Island ? 4.f : 0.f, top = atTop ? edge : height - edge - band;
        return DockHull{20.25f, top, 620.25f, top + band};
    };
    for (bool atTop : {false, true}) {
        char const* side = atTop ? "top" : "bottom";
        char name[80];
        for (auto shape : {DockShape::Island, DockShape::EdgeArc}) {
            snprintf(name, sizeof name, "unified-%s-%s", shape == DockShape::Island ? "island" : "edge-arc", side);
            CheckSmallerTrayCoverage(name, {.shape = shape, .trayReduction = 8}, 12, fit(shape, atTop, 80), atTop, width, height);
            snprintf(name, sizeof name, "separate-%s-%s", shape == DockShape::Island ? "island" : "edge-arc", side);
            CheckSmallerTrayCoverage(name, {.shape = shape, .layout = DockLayout::Separate, .trayReduction = 8}, 12,
                                     fit(shape, atTop, 80), atTop, width, height);
        }
        snprintf(name, sizeof name, "unified-edge-arc-r20-d16-band40-%s", side);
        CheckSmallerTrayCoverage(name, {.shape = DockShape::EdgeArc, .trayReduction = 16}, 20, fit(DockShape::EdgeArc, atTop, 40),
                                 atTop, width, height);
        snprintf(name, sizeof name, "unified-island-r4-d16-%s", side);
        CheckSmallerTrayCoverage(name, {.trayReduction = 16}, 4, fit(DockShape::Island, atTop, 80), atTop, width, height);
    }
    puts("PASS: the Smaller tray trace sits on the hull's far side over the buttons and d inside it over the tray, runs "
         "the contract's S through its centre at half the drop and flat beyond c1 and c2, keeps the screen-edge side "
         "flush, and rounds the tray's far corners at min(r, (H - d)/2)");
    CheckInteriorField("unified-island-bottom", {.trayReduction = 8}, 12, false, true);
    CheckInteriorField("unified-edge-arc-top", {.shape = DockShape::EdgeArc, .trayReduction = 8}, 12, true, true);
    CheckInteriorField("unified-island-r4-d16-bottom", {.trayReduction = 16}, 4, false, false);
    puts("PASS: around the S the shader's field and its optical field are the trace's signed distance within half a "
         "pixel 12 DIP deep, and a 1 or 10 DIP border band is as wide across the S as across the flat far side with "
         "nothing border-coloured deeper than it");
    for (auto layout : {DockLayout::Unified, DockLayout::Separate})
        for (auto shape : {DockShape::Island, DockShape::Edge, DockShape::EdgeArc})
            for (bool atTop : {false, true}) {
                char name[80];
                snprintf(name, sizeof name, "smaller-tray-%s-%s-%s", layout == DockLayout::Separate ? "separate" : "unified",
                         shape == DockShape::Island ? "island" : shape == DockShape::Edge ? "edge" : "edge-arc",
                         atTop ? "top" : "bottom");
                CheckOutlineParity(name, MakeOutline({.shape = shape, .layout = layout, .trayReduction = 8}, 240, 12, 80, atTop, kContent),
                                   fit(shape, atTop, 80), width, height);
            }
    struct Case { char const* name; DockStyle style; float radius, band; };
    Case cases[]{
        {"smaller-tray-unified-edge-arc-r20-d16", {.shape = DockShape::EdgeArc, .trayReduction = 16}, 20, 80}, // rho = r
        {"smaller-tray-unified-edge-arc-r4-d16", {.shape = DockShape::EdgeArc, .trayReduction = 16}, 4, 80},   // rho = d / 2
        {"smaller-tray-unified-edge-arc-r20-d16-band40", {.shape = DockShape::EdgeArc, .trayReduction = 16}, 20, 40}, // tray radius 12
        {"smaller-tray-separate-edge-arc-inner-arcs", {.shape = DockShape::EdgeArc, .layout = DockLayout::Separate, .innerArcs = true,
                                                       .trayReduction = 8}, 12, 80},
    };
    for (auto const& c : cases)
        for (bool atTop : {false, true}) {
            char name[96];
            snprintf(name, sizeof name, "%s-%s", c.name, atTop ? "top" : "bottom");
            CheckOutlineParity(name, MakeOutline(c.style, 240, c.radius, c.band, atTop, kContent), fit(c.style.shape, atTop, c.band), width, height);
        }
    puts("PASS: with Smaller tray the trace and the shader's field agree over Unified/Separate x Island/Edge/Edge arc x "
         "top/bottom, at rho = r, at rho = d/2, with a clamped tray radius and with inner arcs");
    // Drops and the step land in dockPieceFar. Without a drop it packs zeros there, and with one the old arrays are
    // those of the same outline without it but for the screen edge's side, which the S's mirror needs.
    auto packed = [](DockOutline const& outline) { LiquidRenderer::LensConstants lens{}; ApplyDockShape(lens, outline, 1.5f); return lens; };
    for (auto layout : {DockLayout::Unified, DockLayout::Separate})
        for (auto shape : {DockShape::Island, DockShape::Edge, DockShape::EdgeArc})
            for (bool atTop : {false, true}) {
                auto plainOutline = MakeOutline({.shape = shape, .layout = layout}, 240, 12, 80, atTop, kContent);
                auto droppedOutline = MakeOutline({.shape = shape, .layout = layout, .trayReduction = 8}, 240, 12, 80, atTop, kContent);
                assert(NeedsPath(droppedOutline) && !HullOnly(droppedOutline));
                auto plain = packed(plainOutline), dropped = packed(droppedOutline);
                for (size_t i = 0; i < kMaxDockPieces; ++i)
                    for (size_t k = 0; k < 4; ++k) assert(plain.dockPieceFar[i][k] == 0);
                bool separate = layout == DockLayout::Separate;
                assert(dropped.dockShape[0] == 1 && dropped.dockShape[1] == (separate ? 2 : 1) && dropped.dockShape[2] == (atTop ? -1 : 1));
                if (!HullOnly(plainOutline))
                    assert(std::memcmp(plain.dockPieceRect, dropped.dockPieceRect, sizeof plain.dockPieceRect) == 0 &&
                           std::memcmp(plain.dockPieceSide, dropped.dockPieceSide, sizeof plain.dockPieceSide) == 0 &&
                           plain.dockShape[1] == dropped.dockShape[1]);
                auto const* drops = dropped.dockPieceFar[separate ? 1 : 0];
                if (separate) assert(drops[0] == 12 && drops[1] == 12 && drops[2] == 0 && drops[3] == 0 &&
                                     dropped.dockPieceFar[0][0] == 0 && dropped.dockPieceFar[0][1] == 0);
                else assert(drops[0] == 0 && drops[1] == 12 && drops[2] == -(8 + 240 + 6) * 1.5f && drops[3] == 1);
            }
    // The tray wall's flare takes the tray band's radius: radius 20 on a 40 DIP band with 16 off leaves 12.
    auto clamped = packed(MakeOutline({.shape = DockShape::EdgeArc, .trayReduction = 16}, 240, 20, 40, false, kContent));
    assert(clamped.dockPieceSide[0][2] == 30 && clamped.dockPieceSide[0][3] == 18 && clamped.dockPieceFar[0][1] == 24);
    // Facing arcs take the outer ones' radius, the tray bar's clamped like its outer one.
    auto clampedBars = packed(MakeOutline({.shape = DockShape::EdgeArc, .layout = DockLayout::Separate, .innerArcs = true,
                                           .trayReduction = 16}, 240, 20, 40, false, kContent));
    assert(clampedBars.dockPieceSide[0][2] == 30 && clampedBars.dockPieceSide[0][3] == 30 && clampedBars.dockPieceSide[1][2] == 18 &&
           clampedBars.dockPieceSide[1][3] == 18);
    puts("PASS: Smaller tray packs each wall's drop and the S's anchor in dockPieceFar, zeros without it, leaves the old "
         "arrays as they were, and clamps the tray wall's flare to the tray band's radius");
    // 0.4.5: the reduction reaches the taskbar's band less 24 DIP. A 76 DIP taskbar's 68 DIP band allows 44.
    assert(TrayDrop({.trayReduction = 30}, 40) == 16 && TrayDrop({.trayReduction = 30}, 68) == 30 &&
           TrayDrop({.trayReduction = 60}, 68) == 44);
    settingsjson::JsonObject stored;
    stored.Insert(L"smallerTray", settingsjson::JsonValue::CreateBooleanValue(true));
    auto read = [&](double value) {
        stored.Insert(L"trayReduction", settingsjson::JsonValue::CreateNumberValue(value));
        return ReadDockStyle(stored).trayReduction;
    };
    printf("TRAY-REDUCTION ReadDockStyle 30 -> %g, 44 -> %g, 200 -> %g, 0 -> %g; TrayDrop(30) band 40 -> %g, band 68 -> %g\n",
           read(30), read(44), read(200), read(0), TrayDrop({.trayReduction = 30}, 40), TrayDrop({.trayReduction = 30}, 68));
    assert(read(30) == 30 && read(44) == 44 && read(100) == 100 && read(200) == 100 && read(0) == 1);
    puts("PASS: ReadDockStyle reads a stored 30 as 30 within the row's 1..100, and TrayDrop applies 16 of it at band 40 "
         "and all of it at band 68");
    for (bool atTop : {false, true}) {
        char name[80];
        for (float radius : {24.f, 4.f}) { // rho = r, then rho = d / 2
            snprintf(name, sizeof name, "unified-island-r%g-d44-band68-%s", radius, atTop ? "top" : "bottom");
            CheckSmallerTrayCoverage(name, {.trayReduction = 44}, radius, fit(DockShape::Island, atTop, 68), atTop, width, height);
            CheckInteriorField(name, {.trayReduction = 44}, radius, atTop, true, 68);
        }
    }
    Case large[]{
        {"smaller-tray-unified-island-r24-d44-band68", {.trayReduction = 44}, 24, 68},
        {"smaller-tray-unified-island-r4-d44-band68", {.trayReduction = 44}, 4, 68},
        {"smaller-tray-unified-edge-arc-r24-d44-band68", {.shape = DockShape::EdgeArc, .trayReduction = 44}, 24, 68},
        {"smaller-tray-unified-edge-arc-r4-d44-band68", {.shape = DockShape::EdgeArc, .trayReduction = 44}, 4, 68},
        {"smaller-tray-separate-island-r24-d44-band68", {.layout = DockLayout::Separate, .trayReduction = 44}, 24, 68},
    };
    for (auto const& c : large)
        for (bool atTop : {false, true}) {
            char name[96];
            snprintf(name, sizeof name, "%s-%s", c.name, atTop ? "top" : "bottom");
            CheckOutlineParity(name, MakeOutline(c.style, 240, c.radius, c.band, atTop, kContent), fit(c.style.shape, atTop, c.band), width, height);
        }
    puts("PASS: at a 68 DIP band with d = 44 the trace runs the contract's S at rho = r and rho = d/2, the shader's field "
         "is its signed distance 12 DIP deep with an even border band, and the trace and the field agree top and bottom");
}

// A slanted end's content as the contract states it: a plate 8 DIP in from the hull edge and unbounded inward, from
// depth top to bottom, its corners rounded at most to half its height. The first button's spans the band less its
// vertical padding, the last tray item's highlight the tray band below the drop less its own; a padding always leaves
// 1 DIP. An icon's plate is its box, sharp, at its own inset, and only points beside it, at its depths, count: the gap
// a wall leaves beside the icon. Coordinates: u outward from the hull edge, v in from the hull's far side.
struct EndPlate {
    float top, bottom, radius, inset = 8;
    bool beside = false;
    static EndPlate of(float band, float drop, double padding, double corner) {
        float pad = std::clamp(float(padding), 0.f, std::max(0.f, (band - drop - 1) / 2)), top = drop + pad, bottom = band - pad;
        return {top, bottom, std::min(float(corner), (bottom - top) / 2)};
    }
    static EndPlate buttons(float band) { return of(band, 0, kContent.buttonPadding, kContent.buttonRadius); }
    static EndPlate tray(float band, float drop) { return of(band, drop, kContent.trayPadding, kContent.trayRadius); }
    static EndPlate icon(IconBox const& box) { return {float(box.top), float(box.bottom), 0, float(box.inset), true}; }
    float distance(float u, float v) const { // Negative inside.
        float over = std::max({0.f, top + radius - v, v - bottom + radius});
        return beside && over > 0 ? 1e30f : std::hypot(std::max(0.f, u + inset + radius), over) - radius;
    }
};

// Slanted ends (0.4.7) as the contract states them for a Curve angle a, the S's middle measured from the screen edge,
// written out apart from the mod: k = cot a, the wall's lean per DIP of depth, tau = tan(a / 2) and h = sqrt(1 + k^2).
// An outer wall from its far side at d_w to the screen edge at H is L = (H - d_w) h long. Every convex curve of the
// outline rounds at one radius rho and every concave one, Edge arc's flare and the ramp's foot, at another, c, both
// SlantRounding's: Edge arc's far corner and flare take (rho + c) tau of L, an Island's far corner and toe rho
// (tau + 1 / tau), Edge's far corner rho tau. A straight run at the Curve angle takes the rest. The far vertex sits on
// the hull edge moved in by the largest offset that keeps the end the clearance from its plate, kEndSpace or the left
// end's gap beside its icon for a mirrored right end, found here by bisection on the distance.
//
// The radii (0.4.7, the user's pick in the shape preview: every convex curve alike, every concave one alike; 0.4.9, the
// user's: the concave one is its own setting and gives way to the convex one, so each slider moves only its own curves,
// and both layouts round alike, whatever Unified's step holds). The convex rho is the corner radius r as far as each
// end allows a convex fillet alone: each outer wall's L / tau (an Island's, with its convex toe, L / (tau + 1 / tau))
// and a dropped wall's (H - d) / 2. The concave c is Concave radius cr up to the room rho leaves on each Edge arc wall,
// L / tau - rho, and never below 0. Smaller tray's step is SlantedRamp's.
struct SlantRadii { float convex, concave; };
SlantRadii SlantRounding(DockShape shape, float band, float drop, float radius, float concave, float angle) {
    float k = float(1 / std::tan(angle * 3.14159265358979323846 / 180)), tau = float(std::tan(angle * 3.14159265358979323846 / 360));
    float h = std::sqrt(1 + k * k);
    std::vector<float> convexRooms{radius}, concaveRooms;
    for (float run : {band, band - drop}) {
        convexRooms.push_back(run * h / (shape == DockShape::Island ? tau + 1 / tau : tau));
        if (run < band) convexRooms.push_back(run / 2);
        if (shape == DockShape::EdgeArc) concaveRooms.push_back(run * h / tau);
    }
    float rho = *std::min_element(convexRooms.begin(), convexRooms.end()), c = concave;
    for (float room : concaveRooms) c = std::min(c, room - rho);
    return {rho, std::max(0.f, c)};
}
// The Concave radius setting's default, which every style here keeps unless it sets its own.
constexpr float kDefaultConcave = 18;

// The end space (0.4.8, the user's pick in the shape preview over 2, 4 and 8): how far a slanted end and the ramp keep
// outside the content plates.
constexpr float kEndSpace = 6;
// The gap every slanted curve beside an icon leaves beside it (0.4.9, the user's pick over 20, 22 and 28 DIP), moving in
// or out to it: Start's end, the tray's, Separate's facing ends and Unified's step beside the last button's icon.
constexpr float kIconSpace = 24;

struct SlantedEnd {
    DockShape shape;
    EndPlate plate;
    SlantRadii radii;
    float clearance, angle, k, tau, h, band, drop, r, bleed, farRadius, footRadius, offset{}, vertex{}, foot{};
    SlantedEnd(DockShape shape, float band, float drop, float radius, SlantRadii radii, float angle, EndPlate const& plate,
               float clearance = kEndSpace)
        : shape(shape), plate(plate), radii(radii), clearance(clearance), angle(angle),
          k(float(1 / std::tan(angle * 3.14159265358979323846 / 180))),
          tau(float(std::tan(angle * 3.14159265358979323846 / 360))), h(std::sqrt(1 + k * k)), band(band), drop(drop),
          r(drop > 0 ? std::min(radius, (band - drop) / 2) : radius), bleed(shape == DockShape::Island ? 0 : std::max(radius, 11.f)),
          farRadius(radii.convex),
          footRadius(shape == DockShape::EdgeArc ? radii.concave : shape == DockShape::Island ? radii.convex : 0) {
        auto place = [&](float o) { offset = o, vertex = -o, foot = vertex + (band - drop) * k; };
        // Moving in only brings the wall closer, so the clear offsets are an interval up to the largest. A plate's starts
        // at 0, where the end sits on the hull edge; a mirrored end may also move out.
        float lo = plate.beside ? -64 : 0, hi = 128;
        place(lo);
        if (nearest(plate) >= clearance)
            for (int i = 0; i < 30; ++i) {
                place((lo + hi) / 2);
                (nearest(plate) >= clearance ? lo : hi) = (lo + hi) / 2;
            }
        place(lo);
    }
    // The end's least distance to a plate. Beside an icon it is least on the icon's top or bottom row, which the profile's
    // points only come near, so those rows are read off the wall too.
    float nearest(EndPlate const& to) const {
        float least = 1e30f;
        profile(.02f, [&](float u, float v) { least = std::min(least, to.distance(u, v)); });
        if (to.beside)
            for (float row : {to.top, to.bottom})
                if (row >= drop && row <= band) least = std::min(least, to.distance(wall(row), row));
        return least;
    }
    static float circle(float radius, float u) { return std::sqrt(std::max(0.f, radius * radius - u * u)); }
    template <class F> static float column(F const& f, float x) { // The mean over a pixel column: what coverage sums to.
        float sum = 0;
        for (int i = 0; i < 64; ++i) sum += f(x + (i + .5f) / 64);
        return sum / 64;
    }
    // Where the far side leaves the flat.
    float farStart() const { return vertex - farRadius * tau; }
    // The straight run at the Curve angle: from where the far fillet ends to where the toe or the flare begins, or on
    // to the screen edge for Edge. Its length is L less the fillets' tangents.
    float runFromU() const { return farStart() + farRadius / h; }
    float runFromV() const { return drop + farRadius * (1 - k / h); }
    float runToU() const {
        return shape == DockShape::EdgeArc ? footU() - footRadius / h : shape == DockShape::Island ? footU() + footRadius / h : foot;
    }
    float runToV() const {
        return shape == DockShape::EdgeArc ? footV() + footRadius * k / h : shape == DockShape::Island ? footV() - footRadius * k / h : band;
    }
    float run() const {
        float footTangent = shape == DockShape::EdgeArc ? footRadius * tau : shape == DockShape::Island ? footRadius / tau : 0;
        return (band - drop) * h - farRadius * tau - footTangent;
    }
    // The traced run: Edge's goes on past the screen edge through the bleed, max(r, 11) DIP deep.
    float tracedRun() const { return run() + (shape == DockShape::Edge ? bleed * h : 0); }
    float midU() const { return (runFromU() + runToU()) / 2; }
    float midV() const { return (runFromV() + runToV()) / 2; }
    // The foot fillet's centre: the toe's inside the dock, the flare's outside it below the tip.
    float footU() const { return shape == DockShape::EdgeArc ? foot + footRadius * tau : foot - footRadius / tau; }
    float footV() const { return band - footRadius; }
    // The end's outline from the far fillet's start to the toe's or the flare's end on the screen edge, or Edge's foot,
    // as points at most step apart.
    template <class F> void profile(float step, F const& visit) const {
        auto arc = [&](float cu, float cv, float rho, float from, float to) {
            int n = std::max(1, int(std::ceil(rho * std::abs(to - from) / step)));
            for (int i = 0; i <= n; ++i) visit(cu + rho * std::cos(from + (to - from) * i / n), cv + rho * std::sin(from + (to - from) * i / n));
        };
        float pi = 3.14159265f, lean = std::atan(k);
        arc(farStart(), drop + farRadius, farRadius, -pi / 2, -lean);
        int n = std::max(1, int(std::ceil(run() / step)));
        for (int i = 0; i <= n; ++i)
            visit(runFromU() + (runToU() - runFromU()) * i / n, runFromV() + (runToV() - runFromV()) * i / n);
        if (shape == DockShape::EdgeArc) arc(footU(), footV(), footRadius, pi - lean, pi / 2);
        if (shape == DockShape::Island) arc(footU(), footV(), footRadius, -lean, pi / 2);
    }
    // Past a Curve angle of 45 the end runs steeper than 45 degrees from where the far fillet turns past it to where the
    // flare comes back to it, Edge's screen edge, or the toe's outermost point.
    float steepU() const { return farStart() + farRadius * std::sqrt(.5f); }
    float steepV() const { return drop + farRadius * (1 - std::sqrt(.5f)); }
    float shallowU() const { return footU() - footRadius * std::sqrt(.5f); }
    float steepEndV() const {
        return shape == DockShape::EdgeArc ? footV() + footRadius * std::sqrt(.5f) : shape == DockShape::Island ? footV() : band;
    }
    // How far the end reaches past the hull edge on screen: the flare's tip, Edge's foot, the toe's outermost point.
    float footprint() const { return shape == DockShape::Island ? footU() + footRadius : footU(); }
    // The far side's depth at u, over the far fillet, the straight run and on over the flare to the tip; the band past
    // that.
    float depth(float u) const {
        if (u <= farStart()) return drop;
        if (u <= runFromU()) return drop + farRadius - circle(farRadius, u - farStart());
        if (u <= runToU()) return runFromV() + (u - runFromU()) / k;
        if (shape == DockShape::EdgeArc && u <= footU()) return footV() + circle(footRadius, u - footU());
        return band;
    }
    // The wall's u at depth v: the far fillet, the straight run, then the flare or the toe's outer half.
    float wall(float v) const {
        if (v <= runFromV()) return farStart() + circle(farRadius, farRadius - (v - drop));
        if (v <= runToV()) return runFromU() + (v - runFromV()) * k;
        return shape == DockShape::EdgeArc ? footU() - circle(footRadius, v - footV()) : footU() + circle(footRadius, v - footV());
    }
};

// Smaller tray's ramp as the contract states it: an S of a convex fillet of rho and a concave one of c, rho tau and
// c tau along it, and a straight run at the Curve angle between them, so it spans dk + (rho + c) tau from c1 to c2,
// with k, tau and h those of SlantedEnd's angle. Its d h holds (rho + c) tau: rho is the outline's convex radius as far
// as it holds that alone, and c its concave one up to the rest (0.4.9: the user's rule, so a short step rounds no other
// curve tighter). It starts at the tray content's edge and
// moves toward the buttons by the largest shift that keeps it kEndSpace from the last button's plate, which ends 12 DIP
// before the tray content, and kIconSpace beside the last button's icon when it has one (0.4.9), at most until the S ends
// where that plate does; found here by bisection on the distance. It
// never comes nearer the first tray item's highlight than the drop's depth allows. x along the hull.
struct SlantedRamp {
    float angle, k, tau, h, d, rho, foot, gapStart, trayLeft, limit, shift{}, c1{}, c2{};
    EndPlate plate; // The last button's, at the gap's start.
    std::optional<EndPlate> icon; // The last button's icon, from the gap's start.
    // The ends' convex radius and the Concave radius setting itself: the step's foot answers to its own room, not to
    // what the end walls could hold.
    SlantedRamp(DockHull const& hull, float drop, float trayWidth, float angle, float convex, float concave, float band,
                std::optional<IconBox> lastButtonIcon = std::nullopt)
        : angle(angle), k(float(1 / std::tan(angle * 3.14159265358979323846 / 180))),
          tau(float(std::tan(angle * 3.14159265358979323846 / 360))), h(std::sqrt(1 + k * k)), d(drop),
          rho(std::min(convex, d * h / tau)), foot(std::min(concave, d * h / tau - rho)),
          gapStart(hull.right - (8 + trayWidth + 12)),
          trayLeft(hull.right - (8 + trayWidth)), limit(trayLeft + drop * k + (rho + foot) * tau - gapStart), plate(EndPlate::buttons(band)) {
        plate.inset = 0;
        // The icon's inset counts from the bar's edge, kDockPadding past the last button.
        if (lastButtonIcon) icon = EndPlate::icon(*lastButtonIcon), icon->inset -= 8;
        auto clear = [&] { return nearest() >= kEndSpace && iconGap() >= kIconSpace; };
        float lo = 0, hi = limit;
        place(0);
        if (clear())
            for (int i = 0; i < 30; ++i) {
                place((lo + hi) / 2);
                (clear() ? lo : hi) = (lo + hi) / 2;
            }
        place(lo);
    }
    float iconGap() const {
        float least = 1e30f;
        if (icon) profile(.02f, [&](float x, float v) { least = std::min(least, icon->distance(x - gapStart, v)); });
        return least;
    }
    void place(float o) { shift = o, c1 = trayLeft - o, c2 = c1 + d * k + (rho + foot) * tau; }
    float nearest() const {
        float least = 1e30f;
        profile(.02f, [&](float x, float v) { least = std::min(least, plate.distance(x - gapStart, v)); });
        return least;
    }
    // The straight run, from where the first arc ends to where the second begins: d h less both arcs' tangents.
    float runFromX() const { return c1 + rho / h; }
    float runFromV() const { return rho * (1 - k / h); }
    float runToX() const { return c2 - foot / h; }
    float runToV() const { return d - foot * (1 - k / h); }
    float run() const { return d * h - rho * tau - foot * tau; }
    float midX() const { return (runFromX() + runToX()) / 2; }
    float midV() const { return (runFromV() + runToV()) / 2; }
    float depth(float x) const {
        if (x <= c1) return 0;
        if (x <= runFromX()) return rho - SlantedEnd::circle(rho, x - c1);
        if (x <= runToX()) return runFromV() + (x - runFromX()) / k;
        if (x <= c2) return d - foot + SlantedEnd::circle(foot, x - c2);
        return d;
    }
    // Its x at depth v, on the first arc, the straight run and the second arc below.
    float wall(float v) const {
        if (v <= runFromV()) return c1 + SlantedEnd::circle(rho, rho - v);
        if (v <= runToV()) return runFromX() + (v - runFromV()) * k;
        return c2 - SlantedEnd::circle(foot, v - (d - foot));
    }
    // The S from c1 to c2 as points at most step apart.
    template <class F> void profile(float step, F const& visit) const {
        float lean = std::atan(1 / k);
        auto arc = [&](float cx, float cv, float radius, float from, float to, float sign) {
            int n = std::max(1, int(std::ceil(radius * std::abs(to - from) / step)));
            for (int i = 0; i <= n; ++i) {
                float a = from + (to - from) * i / n;
                visit(cx + sign * radius * std::sin(a), cv - sign * radius * std::cos(a));
            }
        };
        arc(c1, rho, rho, 0, lean, 1);
        int n = std::max(1, int(std::ceil(run() / step)));
        for (int i = 0; i <= n; ++i) visit(runFromX() + (runToX() - runFromX()) * i / n, runFromV() + (runToV() - runFromV()) * i / n);
        arc(c2, d - foot, foot, lean, 0, -1);
    }
};

// A trace's coverage read in the outline's frame: x along the hull, depth in from its far side, which is the hull's
// bottom on a top taskbar. A depth row is the pixel row spanning [depth, depth + 1]; the far side is on a pixel edge.
struct OutlineRaster {
    DockOutline outline;
    std::vector<float> coverage;
    UINT width, height;
    int farLine;
    bool atTop;
    OutlineRaster(DockOutline const& outline, DockHull const& hull, UINT width, UINT height)
        : outline(outline), coverage(TraceCoverage(outline, hull, width, height)), width(width), height(height),
          farLine(int(outline.atTop ? hull.bottom : hull.top)), atTop(outline.atTop) {}
    float at(int x, int depth) const {
        int y = atTop ? farLine - depth - 1 : farLine + depth;
        return x < 0 || y < 0 || x >= int(width) || y >= int(height) ? 0 : coverage[size_t(y) * width + size_t(x)];
    }
    // Column x's uncovered run in from the far side: where the far side crosses it, as a column mean.
    float farDepth(int x, int through) const { float sum = 0; for (int v = 0; v < through; ++v) sum += 1 - at(x, v); return sum; }
    float covered(int x, int from, int to) const { float sum = 0; for (int v = from; v < to; ++v) sum += at(x, v); return sum; }
    // A depth row's left wall between an outside column x0 and an inside one x1, and a right wall between an inside x0
    // and an outside x1: the wall's mean x over the row.
    float leftWall(int depth, int x0, int x1) const { float x = float(x0); for (int c = x0; c < x1; ++c) x += 1 - at(c, depth); return x; }
    float rightWall(int depth, int x0, int x1) const { float x = float(x0); for (int c = x0; c < x1; ++c) x += at(c, depth); return x; }
};

// The kinds of the calls a trace makes, in order: the structure a path morph needs identical at both keyframes.
struct CallKinds : ID2D1GeometrySink {
    std::string kinds;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, void**) override { return E_NOINTERFACE; }
    ULONG STDMETHODCALLTYPE AddRef() override { return 1; }
    ULONG STDMETHODCALLTYPE Release() override { return 1; }
    void STDMETHODCALLTYPE SetFillMode(D2D1_FILL_MODE) override { kinds += 'F'; }
    void STDMETHODCALLTYPE SetSegmentFlags(D2D1_PATH_SEGMENT) override { kinds += 'S'; }
    void STDMETHODCALLTYPE BeginFigure(D2D1_POINT_2F, D2D1_FIGURE_BEGIN) override { kinds += 'B'; }
    void STDMETHODCALLTYPE AddLines(D2D1_POINT_2F const*, UINT32) override { kinds += 'l'; }
    void STDMETHODCALLTYPE AddBeziers(D2D1_BEZIER_SEGMENT const*, UINT32) override { kinds += 'c'; }
    void STDMETHODCALLTYPE EndFigure(D2D1_FIGURE_END) override { kinds += 'E'; }
    HRESULT STDMETHODCALLTYPE Close() override { return S_OK; }
    void STDMETHODCALLTYPE AddLine(D2D1_POINT_2F) override { kinds += 'L'; }
    void STDMETHODCALLTYPE AddBezier(D2D1_BEZIER_SEGMENT const*) override { kinds += 'C'; }
    void STDMETHODCALLTYPE AddQuadraticBezier(D2D1_QUADRATIC_BEZIER_SEGMENT const*) override { kinds += 'Q'; }
    void STDMETHODCALLTYPE AddQuadraticBeziers(D2D1_QUADRATIC_BEZIER_SEGMENT const*, UINT32) override { kinds += 'q'; }
    void STDMETHODCALLTYPE AddArc(D2D1_ARC_SEGMENT const*) override { kinds += 'A'; }
};

// The shader's field against the trace's own signed distance at production scale over x windows (DIP): the plain and
// the optical field within half a pixel 12 DIP deep, and border bands of 1, 8 and 10 DIP of even width: every pixel
// nearer the outline than w - 1 px fully border-coloured, none deeper than w + 1 px coloured at all.
void CheckWindowedField(char const* name, DockOutline const& outline, DockHull const& hull,
                        std::vector<std::pair<float, float>> const& windows, bool measureOnly = false) {
    constexpr float scale = 1.5f, deep = 12 * scale, widths[]{1, 8, 10};
    UINT width = 800, height = 132;
    LiquidRenderer::LensConstants lens{};
    lens.imageSize[0] = float(width); lens.imageSize[1] = float(height);
    lens.center[0] = (hull.left + hull.right) / 2 * scale; lens.center[1] = (hull.top + hull.bottom) / 2 * scale;
    lens.halfSize[0] = (hull.right - hull.left) / 2 * scale; lens.halfSize[1] = (hull.bottom - hull.top) / 2 * scale;
    lens.radius = outline.radius * scale;
    ApplyDockShape(lens, outline, scale);
    auto field = DockField(lens, width, height), optical = DockField(lens, width, height, true);
    float worst = 0, worstOptical = 0;
    size_t pixels = 0, thin[3]{}, deepBorder[3]{};
    for (auto [from, to] : windows) {
        TraceDistance trace(outline, hull, scale, {from - 20, -20, to + 20, 120});
        for (UINT y = 0; y < height; ++y)
            for (UINT x = UINT(std::max(0.f, from * scale)); x < UINT(std::min(float(width), to * scale)); ++x) {
                float t = trace(x + .5f, y + .5f), f = field[size_t(y) * width + x][0];
                if (t >= 0) continue;
                ++pixels;
                if (-t <= deep) {
                    worst = std::max(worst, std::abs(f - t));
                    worstOptical = std::max(worstOptical, std::abs(optical[size_t(y) * width + x][0] - t));
                }
                for (size_t k = 0; k < 3; ++k) {
                    float px = widths[k] * scale; // The shader colours smoothstep(-w - 0.5, -w + 0.5, field) of the border.
                    if (-t <= px - 1 && f < -px + .5f) ++thin[k];
                    if (-t > px + 1 && f > -px - .5f) ++deepBorder[k];
                }
            }
    }
    printf("INTERIOR-WINDOWS %s pixels=%zu worst field error %.3f px, optical %.3f px, %g px deep; border w=1/8/10 DIP: "
           "thin %zu/%zu/%zu, deep %zu/%zu/%zu\n", name, pixels, worst, worstOptical, deep, thin[0], thin[1], thin[2],
           deepBorder[0], deepBorder[1], deepBorder[2]);
    if (measureOnly) return;
    assert(pixels > 0 && worst <= .5f && worstOptical <= .5f);
    for (size_t k = 0; k < 3; ++k) assert(thin[k] == 0 && deepBorder[k] == 0);
}

void RunSlantedOutline() {
    // Wide enough for Separate at 45 degrees, whose facing ends part the bars by some 200 DIP.
    constexpr UINT width = 1100, height = 420;
    // The Curve angle is the S's middle from the screen edge; DockStyle::slant is the walls' lean from upright.
    auto slanted = [](DockStyle style, float angle) { style.slant = 90 - angle; return style; };
    // Revision 1's 45-degree results keep their names; the other angles tag theirs.
    auto tagged = [](std::string name, float angle) { return angle == 45 ? name : name + "-angle" + std::to_string(int(angle)); };
    // 70 is the default and 45 Revision 1's angle. The steepest, 85, runs the Island ends and the ramp.
    constexpr float angles[]{45, 70, 85};
    auto runs = [](float angle, DockShape shape, DockLayout layout, double drop) {
        return angle != 85 || shape == DockShape::Island || (layout == DockLayout::Unified && drop > 0);
    };
    int failures = 0, movedOut = 0;
    // Intent asserts report every failure before the run stops, so a mutant's log names each one it breaks.
    auto expect = [&](bool ok, char const* name, char const* what) {
        if (!ok) { ++failures; printf("SLANTED-FAIL %s: %s\n", name, what); }
    };
    // ReadDockStyle is the one place that maps the Curve angle to the lean: 90 less the angle clamped to 30..85, and
    // nothing at all with Slanted ends off.
    {
        auto read = [](std::optional<bool> on, std::optional<double> angle) {
            settingsjson::JsonObject stored;
            if (on) stored.Insert(L"slantedEnds", settingsjson::JsonValue::CreateBooleanValue(*on));
            if (angle) stored.Insert(L"slantAngle", settingsjson::JsonValue::CreateNumberValue(*angle));
            return ReadDockStyle(stored).slant;
        };
        double byDefault = read(true, {}), at45 = read(true, 45), at30 = read(true, 30), at85 = read(true, 85);
        double above = read(true, 200), below = read(true, 10), off = read(false, 45), absent = read({}, {});
        printf("SLANTED-STYLE ReadDockStyle slantedEnds true -> %g (Curve angle 45 -> %g, 30 -> %g, 85 -> %g, 200 -> %g, 10 -> %g), "
               "false -> %g, absent -> %g\n", byDefault, at45, at30, at85, above, below, off, absent);
        assert(byDefault == 20 && at45 == 45 && at30 == 60 && at85 == 5 && above == 5 && below == 60 && off == 0 && absent == 0);
    }
    // Packing. Slant off packs zeros in the polygon arrays and keeps dockShape.x at 0 or 1 over the whole matrix; slant
    // on packs (2, bars, the screen edge's side, 0) and each bar's corner count.
    auto packed = [](DockOutline const& outline) { LiquidRenderer::LensConstants lens{}; ApplyDockShape(lens, outline, 1.5f); return lens; };
    auto zero = [](auto const& array) {
        auto const* bytes = reinterpret_cast<unsigned char const*>(&array);
        return std::all_of(bytes, bytes + sizeof array, [](unsigned char b) { return b == 0; });
    };
    for (float angle : {45.f, 70.f}) {
        int offOutlines = 0, onOutlines = 0;
        for (auto layout : {DockLayout::Unified, DockLayout::Separate})
            for (auto shape : {DockShape::Island, DockShape::Edge, DockShape::EdgeArc})
                for (bool atTop : {false, true})
                    for (double reduction : {0.0, 8.0})
                        for (bool arcs : {false, true}) {
                            DockStyle style{.shape = shape, .layout = layout, .innerArcs = arcs, .trayReduction = reduction};
                            auto off = packed(MakeOutline(style, 240, 12, 80, atTop, kContent));
                            ++offOutlines;
                            expect(zero(off.dockPolygons) && zero(off.dockCorners) && zero(off.dockFillets) &&
                                   (off.dockShape[0] == 0 || off.dockShape[0] == 1), "packing", "slant off packs a polygon");
                            auto outline = MakeOutline(slanted(style, angle), 240, 12, 80, atTop, kContent);
                            auto on = packed(outline);
                            ++onOutlines;
                            size_t bars = layout == DockLayout::Separate ? 2 : 1, corners = 0;
                            for (auto const& polygon : outline.polygons) corners += polygon.size();
                            expect(outline.pieces.empty() && outline.polygons.size() == bars && NeedsPath(outline) && !HullOnly(outline) &&
                                   on.dockShape[0] == 2 && on.dockShape[1] == bars && on.dockShape[2] == (atTop ? -1 : 1) &&
                                   on.dockShape[3] == 0 && on.dockPolygons[0] == outline.polygons[0].size() &&
                                   on.dockPolygons[1] == (bars == 2 ? outline.polygons[1].size() : 0) && corners <= kMaxDockCorners,
                                   "packing", "slant on packs (2, bars, side, 0) and the corner counts");
                        }
        // One named corner at scale 1.5: Unified Island's far-left corner sits the contract's offset in from the hull's
        // left edge at depth 0 with the far radius, and its fillet leaves the leaning wall rho tau up it and joins the
        // far side rho tau along it. Its toe sits the band's lean further out at the band's depth, its fillet r / tau
        // along the edge side and up the wall.
        auto lens = packed(MakeOutline(slanted({}, angle), 240, 12, 80, false, kContent));
        SlantedEnd e(DockShape::Island, 80, 0, 12, SlantRounding(DockShape::Island, 80, 0, 12, kDefaultConcave, angle), angle,
                     EndPlate::buttons(80));
        float t = e.farRadius * e.tau * 1.5f, toe = e.footRadius / e.tau * 1.5f, x = -e.vertex * 1.5f, up = 1 / e.h, out = e.k / e.h;
        float corner[]{x, 0, 0, e.farRadius * 1.5f}, fillet[]{-t * out, t * up, t, 0},
              toeCorner[]{x - 120 * e.k, 120, 0, e.footRadius * 1.5f},
              toeFillet[]{toe, 0, toe * out, -toe * up};
        auto same = [](float const* a, float const* b) { for (int i = 0; i < 4; ++i) if (std::abs(a[i] - b[i]) > 1e-3f) return false; return true; };
        printf("SLANTED-PACKING%s %d outlines off, %d on; unified-island far-left corner (%g, %g, %g, %g) fillet (%.3f, %.3f, %.3f, %.3f), "
               "toe (%g, %g, %g, %g) fillet (%.3f, %.3f, %.3f, %.3f)\n", tagged("", angle).c_str(), offOutlines, onOutlines,
               lens.dockCorners[0][0], lens.dockCorners[0][1], lens.dockCorners[0][2], lens.dockCorners[0][3], lens.dockFillets[0][0],
               lens.dockFillets[0][1], lens.dockFillets[0][2], lens.dockFillets[0][3], lens.dockCorners[3][0], lens.dockCorners[3][1],
               lens.dockCorners[3][2], lens.dockCorners[3][3], lens.dockFillets[3][0], lens.dockFillets[3][1], lens.dockFillets[3][2],
               lens.dockFillets[3][3]);
        expect(lens.dockPolygons[0] == 4 && same(lens.dockCorners[0], corner) && same(lens.dockFillets[0], fillet) &&
               same(lens.dockCorners[3], toeCorner) && same(lens.dockFillets[3], toeFillet), "packing", "the named corners' contract values");
    }
    // Every slanted hull leaves room for the feet: the widest, Edge and Edge arc at r 12 on band 80, reach 181 DIP out.
    auto slantFit = [&](DockShape shape, bool atTop, float band) {
        float edge = shape == DockShape::Island ? 4.f : 0.f, top = atTop ? edge : height - edge - band;
        return DockHull{200.25f, top, 640.25f, top + band};
    };
    // Separate's hull around 100 DIP of buttons, the outline's gap and its 240 DIP tray, as Plan would place them: a
    // fixed hull would leave a wide gap no buttons at all.
    auto separateFit = [&](DockHull hull, DockOutline const& outline) {
        hull.right = hull.left + float(kDockPadding + 100 + outline.gap + 240 + kDockPadding);
        return hull;
    };
    auto side = [](bool atTop) { return atTop ? "top" : "bottom"; };
    auto shapeName = [](DockShape shape) { return shape == DockShape::Island ? "island" : shape == DockShape::Edge ? "edge" : "edge-arc"; };
    // Control: Direct2D's own circle, read by columns the way the S checks read a trace, from slope 0 to about 50
    // degrees: what its antialiasing and flattening give a true arc, with none of the mod's geometry involved. The
    // radii span the toes' 12 to Edge's far fillets' 240; each half sits on its own circle so the large ones fit.
    for (float r : {12.f, 20.f, 120.f, 240.f}) {
        float cx = 420.25f, worst[2]{}, slope[2]{};
        for (bool top : {true, false}) {
            float cy = top ? r + 4.25f : height - r - 3.75f;
            winrt::com_ptr<ID2D1EllipseGeometry> circle;
            winrt::check_hresult(OutlineFactory()->CreateEllipseGeometry(D2D1::Ellipse({cx, cy}, r, r), circle.put()));
            auto coverage = GeometryCoverage(circle.get(), width, height);
            int y0 = int(cy - r) - 2, y1 = int(cy + r) + 3;
            for (int x = int(cx - .75f * r); x + 1 <= cx; ++x) {
                float sum = 0;
                if (top) for (int y = y0; y < int(cy); ++y) sum += 1 - coverage[size_t(y) * width + x];
                else for (int y = int(cy) + 1; y < y1; ++y) sum += 1 - coverage[size_t(y) * width + x];
                float contract = SlantedEnd::column([&](float u) { return top ? cy - SlantedEnd::circle(r, u - cx) - y0
                                                                                : y1 - cy - SlantedEnd::circle(r, u - cx); }, float(x));
                if (float error = std::abs(sum - contract); error > worst[top]) {
                    float run = cx - (x + .5f);
                    worst[top] = error, slope[top] = std::atan2(run, SlantedEnd::circle(r, run)) * 180 / 3.14159265f;
                }
            }
        }
        printf("SLANTED-RASTER-ARC Direct2D's own circle r=%g read by columns: worst %.3f px on its top at %.1f degrees, %.3f px on its "
               "bottom at %.1f degrees\n", r, worst[1], slope[1], worst[0], slope[0]);
    }
    // The trace's own points, 0.05 DIP apart at a 0.001 DIP flattening tolerance, in the outline's frame: geometry
    // apart from Direct2D's raster.
    auto traced = [](DockOutline const& outline, DockHull const& hull) {
        auto geometry = OutlineGeometry(outline, hull);
        float length{}, farLine = outline.atTop ? hull.bottom : hull.top;
        winrt::check_hresult(geometry->ComputeLength(nullptr, .001f, &length));
        std::vector<std::array<float, 2>> points;
        for (float l = 0; l < length; l += .05f) {
            D2D1_POINT_2F point{};
            winrt::check_hresult(geometry->ComputePointAtLength(l, nullptr, .001f, &point, nullptr));
            points.push_back({point.x, outline.atTop ? farLine - point.y : point.y - farLine});
        }
        return points;
    };
    // How far the traced points in the box [x0, x1] x [v0, v1] lie off the contract's curve; no point is a failure.
    auto off = [](std::vector<std::array<float, 2>> const& points, float x0, float x1, float v0, float v1, auto const& distance) {
        float worst = -1;
        for (auto [x, v] : points)
            if (x >= x0 && x <= x1 && v >= v0 && v <= v1) worst = std::max(worst, std::abs(distance(x, v)));
        return worst < 0 ? 1e9f : worst;
    };
    // The trace's own steepness where it passes nearest (x, v) in the outline's frame, in degrees from the hull's
    // direction, and the length of trace either side that keeps it within 0.02 degrees: the straight run there. It is
    // probed at a 1e-5 DIP flattening tolerance, where a flattened chord turns under 0.1 degree even on a 12 DIP toe and
    // the first chord off a 12 to 60 DIP fillet's tangent point turns 0.03 to 0.07 degrees from it; the coarse points
    // only find where to probe.
    auto steepness = [](DockOutline const& outline, DockHull const& hull, std::vector<std::array<float, 2>> const& points, float x, float v) {
        size_t nearest = 0;
        for (size_t i = 0; i < points.size(); ++i)
            if (std::hypot(points[i][0] - x, points[i][1] - v) < std::hypot(points[nearest][0] - x, points[nearest][1] - v)) nearest = i;
        auto geometry = OutlineGeometry(outline, hull);
        float farLine = outline.atTop ? hull.bottom : hull.top, best = nearest * .05f, miss = 1e30f;
        D2D1_POINT_2F tangent{};
        auto probe = [&](float l) {
            D2D1_POINT_2F point{};
            winrt::check_hresult(geometry->ComputePointAtLength(std::max(0.f, l), nullptr, 1e-5f, &point, &tangent));
            return std::hypot(point.x - x, (outline.atTop ? farLine - point.y : point.y - farLine) - v);
        };
        for (float step : {.05f, .005f, .0005f}) {
            float from = best;
            for (int i = -20; i <= 20; ++i)
                if (float m = probe(from + i * step); m < miss) miss = m, best = from + i * step;
        }
        auto degrees = [&](float l) { probe(l); return std::atan2(std::abs(tangent.y), std::abs(tangent.x)) * 180 / 3.14159265f; };
        // A left wall's run closes its figure, so it may end where the path does; probes past either end clamp there.
        float length{};
        winrt::check_hresult(geometry->ComputeLength(nullptr, 1e-5f, &length));
        best = std::clamp(best, 0.f, length);
        // The direction there by the tangents 0.001 DIP either side: a full S's run is a line of rounding length, whose
        // own tangent Direct2D reports when it is probed.
        float at = (degrees(std::max(0.f, best - .001f)) + degrees(std::min(length, best + .001f))) / 2, ends[2]{best, best};
        for (int side = 0; side < 2; ++side)
            for (float step : {1.f, .1f, .01f})
                for (float next = ends[side] + (side ? step : -step); next >= 0 && next <= length && std::abs(degrees(next) - at) <= .02f;
                     next += side ? step : -step)
                    ends[side] = next;
        return std::array<float, 2>{at, ends[1] - ends[0]};
    };
    // Raster budgets: an upright wall reads within 0.07 px by rows. Direct2D's own circle reads up to 0.27 px off by
    // columns near 45 degrees at r 120 (the control above), so an S read there takes the parity rim's 0.4 px, as
    // Smaller tray's S does. The trace's own points must lie on the contract's lines and circles.
    constexpr float straightBudget = .07f, arcBudget = .4f, geometryBudget = .01f;
    // The trace draws a fillet as ceil(sweep / 90 degrees) cubics with handles 4/3 tan(a / 4) of the radius for each
    // cubic's sweep a, as the contract states, and such a cubic leaves its circle by up to this fraction of the radius:
    // 2.7e-4 at 90 degrees, 6e-5 at 70, 4e-6 at 45. The trace's points get that on top of the geometry budget.
    auto cubicError = [](float sweepDegrees) {
        double a = sweepDegrees * 3.14159265358979323846 / 180 / std::ceil(sweepDegrees / 90 - 1e-6), k = 4.0 / 3 * std::tan(a / 4), worst = 0;
        double p[4][2]{{1, 0}, {1, k}, {std::cos(a) + k * std::sin(a), std::sin(a) - k * std::cos(a)}, {std::cos(a), std::sin(a)}};
        for (int i = 0; i <= 400; ++i) {
            double t = i / 400.0, w[4]{(1 - t) * (1 - t) * (1 - t), 3 * t * (1 - t) * (1 - t), 3 * t * t * (1 - t), t * t * t};
            double bx = 0, by = 0;
            for (int j = 0; j < 4; ++j) bx += w[j] * p[j][0], by += w[j] * p[j][1];
            worst = std::max(worst, std::abs(std::hypot(bx, by) - 1));
        }
        return float(worst);
    };
    // An end's traced points, from where its far side leaves the flat out to its footprint, and their least distance to a
    // plate. The flat far side is the band's own: it sits the button padding from the plates, under the end space once
    // that is the larger (4 DIP of padding against 0.4.8's 6), and isn't the wall's to keep clear.
    // `at` is the end's edge when it isn't the hull's: a facing end's bar edge, kDockPadding from its content.
    auto nearestTo = [](DockHull const& hull, std::vector<std::array<float, 2>> const& points, SlantedEnd const& e, bool right,
                        EndPlate const& plate, float at = NAN) {
        float edge = !std::isnan(at) ? at : right ? hull.right : hull.left, sign = right ? 1.f : -1.f, least = 1e30f;
        for (auto [px, v] : points)
            if (float pu = sign * (px - edge); pu >= e.farStart() && pu <= e.footprint() + 1) least = std::min(least, plate.distance(pu, v));
        return least;
    };
    // Clearance: every traced point of an end keeps its clearance from the end's plate within 0.05: kEndSpace from the
    // content plate, so the outline holds the plate grown by it, or a mirrored right end the left end's distance to its icon
    // from the last tray icon. It is tight: the nearest point is within 0.1 DIP of the clearance. Both hold unless the
    // offset is 0: a wall never moves out, and a corner radius past half the band (r 60 on band 68, which the live
    // dock's clamp never passes) rounds the far fillet into the clearance at the hull edge already.
    auto checkClearance = [&](char const* name, DockOutline const& outline, DockHull const& hull,
                              std::vector<std::array<float, 2>> const& points, SlantedEnd const& e, bool right, float at = NAN) {
        float least = nearestTo(hull, points, e, right, e.plate, at);
        // The mod's far corner on this side, the first within 100 DIP of the hull's right edge there; a facing end's isn't
        // looked up.
        float mod = !std::isnan(at) ? NAN : right ? 0 : outline.polygons.front()[0].x.offset;
        if (right && std::isnan(at))
            for (auto const& corner : outline.polygons.back())
                if (corner.x.edge == HullEdge::Right && corner.x.offset > -100) { mod = -corner.x.offset; break; }
        printf("SLANTED-CLEARANCE %s %s end: offset %.3f DIP (mod %.3f), nearest point %.3f DIP from the plate (depth %g to %g, "
               "radius %g, inset %g; clearance %.3f)\n", name, right ? "right" : "left", e.offset, mod, least, e.plate.top,
               e.plate.bottom, e.plate.radius, e.plate.inset, e.clearance);
        expect(e.offset == 0 || least >= e.clearance - .05f, name, "clearance: the outline keeps its clearance outside the plate");
        expect(e.offset == 0 || least <= e.clearance + .1f, name, "clearance: the wall moves in until it is its clearance from the plate");
    };
    // The mod's far corner and concave flare radii for an end: an Edge arc flare takes SlantRounding's concave radius and
    // the far corner its convex one.
    auto checkConcave = [&](char const* name, DockOutline const& outline, SlantedEnd const& e, bool right) {
        auto const& polygon = right ? outline.polygons.back() : outline.polygons.front();
        auto edge = right ? HullEdge::Right : HullEdge::Left;
        float farRadius = right ? -1.f : polygon[0].radius, flare = -1;
        // A flare left no room is a sharp corner, radius 0. The left wall closes the polygon, so its flare is the last
        // corner. On the right the tray bar's facing wall in Separate comes later, on the same edge, so the outer flare
        // is the first there, after its far corner.
        if (!right && polygon.back().x.edge == edge && polygon.back().depth == e.band) flare = polygon.back().radius;
        for (size_t i = 0; right && i < polygon.size() && flare < 0; ++i) {
            auto const& c = polygon[i];
            if (c.x.edge != edge || c.depth != e.band) continue;
            flare = c.radius;
            farRadius = polygon[i - 1].radius;
        }
        printf("SLANTED-CONCAVE %s %s end: far %.3f flare %.3f (ratio %.4f), contract far %.3f flare %.3f\n", name,
               right ? "right" : "left", farRadius, flare, flare / farRadius, e.farRadius, e.footRadius);
        expect(std::abs(flare - e.footRadius) <= .01f && std::abs(farRadius - e.farRadius) <= .01f, name,
               "concave: the flare takes the concave radius and the far corner the convex one, each as it fits");
    };
    // One slanted end against its contract, read off the trace's raster and off its own points: the far side holds its
    // depth up to where the far fillet starts; the raster follows the far fillet, the straight run and on over an Edge
    // arc's flare to its tip or Edge's to its foot, by columns where it is at most 45 degrees steep and by rows where it
    // is steeper (an Island's toe out to its outermost point); every traced point between the far side and the screen
    // edge lies on the far circle, the run's line or the foot circle, at SlantRounding's radius; the trace runs
    // straight at the Curve angle for the contract's run; and its outermost point on screen is the contract's footprint.
    // Then the end's clearance.
    auto checkEnd = [&](char const* name, OutlineRaster const& raster, DockHull const& hull,
                        std::vector<std::array<float, 2>> const& points, SlantedEnd const& e, bool right, float at = NAN) {
        float edge = !std::isnan(at) ? at : right ? hull.right : hull.left, sign = right ? 1.f : -1.f;
        auto u = [&](float x) { return sign * (x - edge); };
        auto x = [&](float u) { return edge + sign * u; };
        auto columns = [&](float from, float to, auto const& visit) { // Whole columns whose u lies in [from, to].
            float x0 = std::min(x(from), x(to)), x1 = std::max(x(from), x(to));
            for (int c = int(std::ceil(x0)); c + 1 <= x1; ++c) visit(c);
        };
        // An Island's columns stop above its toe's centre, clear of the gap under the toe's far half.
        int through = int(e.shape == DockShape::Island ? e.band - e.footRadius : e.band);
        float flat = 0, profile = 0, profileAt = 0;
        columns(e.farStart() - 30, e.farStart(), [&](int c) { flat = std::max(flat, std::abs(raster.farDepth(c, through) - e.drop)); });
        auto profileColumns = [&](float from, float to) {
            columns(from, to, [&](int c) {
                float contract = SlantedEnd::column([&](float px) { return e.depth(u(px)); }, float(c));
                if (float error = std::abs(raster.farDepth(c, through) - contract); error > profile) profile = error, profileAt = u(c + .5f);
            });
        };
        bool steep = e.angle > 45;
        float shallowEnd = e.shape == DockShape::Island ? e.runFromU() : e.footprint() + 2;
        profileColumns(e.farStart(), steep ? e.steepU() : shallowEnd);
        if (steep && e.shape == DockShape::EdgeArc) profileColumns(e.shallowU(), shallowEnd);
        // Each depth row's wall, its mean x over the row, between columns 6 px either side of the contract's.
        float rowProfile = 0, rowAt = 0;
        int rowFrom = int(std::ceil(steep ? e.steepV() : e.runFromV())), rows = 0;
        for (int v = rowFrom; v + 1 <= e.steepEndV(); ++v, ++rows) {
            float contract = SlantedEnd::column([&](float depth) { return x(e.wall(depth)); }, float(v));
            int x0 = int(std::floor(contract)) - 6, x1 = int(std::ceil(contract)) + 6;
            float wall = right ? raster.rightWall(v, x0, x1) : raster.leftWall(v, x0, x1);
            if (float error = std::abs(wall - contract); error > rowProfile) rowProfile = error, rowAt = v + .5f;
        }
        float circleBudget = geometryBudget + std::max(e.farRadius * cubicError(e.angle),
                                                       e.footRadius * cubicError(e.shape == DockShape::Island ? 180 - e.angle : e.angle));
        float flatOff = 0, curveOff = 0, reach = -1e9f;
        size_t flatPoints = 0, curvePoints = 0;
        for (auto [px, v] : points) {
            float pu = u(px);
            if (pu < e.farStart() - 30 || pu > e.footprint() + 1) continue;
            if (v <= e.band + 1e-3f) reach = std::max(reach, pu);
            if (pu <= e.farStart()) {
                if (v < (e.drop + e.band) / 2) flatOff = std::max(flatOff, std::abs(v - e.drop)), ++flatPoints;
            } else if (v > e.drop + 1e-3f && v < e.band - 1e-3f) {
                float off = std::min(std::abs(std::hypot(pu - e.farStart(), v - e.drop - e.farRadius) - e.farRadius),
                                     std::abs(pu - e.runFromU() - e.k * (v - e.runFromV())) / e.h);
                if (e.footRadius > 0) off = std::min(off, std::abs(std::hypot(pu - e.footU(), v - e.footV()) - e.footRadius));
                curveOff = std::max(curveOff, off);
                ++curvePoints;
            }
        }
        auto [tangent, run] = steepness(raster.outline, hull, points, x(e.midU()), e.midV());
        printf("SLANTED-S %s %s end: rho %.2f/%.2f (r_w %g), offset %.3f, far side flat to %.2f: raster %.3f px, trace %.4f DIP; S raster "
               "worst %.3f px at u %.2f; trace off its circles and line %.4f DIP over %zu points; straight run %.3f DIP (contract %.3f) at "
               "%.3f deg through (%.2f, %.2f); footprint %.3f (contract %.3f)\n", name, right ? "right" : "left", e.farRadius, e.footRadius,
               e.r, e.offset, e.farStart(), flat, flatOff, profile, profileAt, curveOff, curvePoints, run, e.tracedRun(), tangent, e.midU(),
               e.midV(), reach, e.footprint());
        if (rows)
            printf("SLANTED-S-ROWS %s %s end: %d rows from depth %d steeper than 45 degrees, wall worst %.3f px at depth %.1f; circles "
                   "budget %.4f DIP\n", name, right ? "right" : "left", rows, rowFrom, rowProfile, rowAt, circleBudget);
        expect(flatPoints > 0 && flat <= .05f && flatOff <= geometryBudget, name, "S: the far side is flat up to where the far fillet starts");
        expect(profile <= arcBudget && rowProfile <= arcBudget, name, "S: the raster follows the contract's S");
        expect(curvePoints > 0 && curveOff <= circleBudget, name,
               "S: every point to the screen edge on the far circle, the run's line or the foot circle, at the outline's one radius");
        // A curve its fillets fill (0.4.9: a concave radius up to the room left) has no run to lean or measure: the probe
        // would find another wall's. The circles above hold it.
        expect(e.tracedRun() <= .1f || (std::abs(tangent - e.angle) <= .5f && std::abs(run - e.tracedRun()) <= .1f), name,
               "S: the straight run is the contract's length, at the Curve angle where there is one");
        expect(std::abs(reach - e.footprint()) <= .06f, name, "S: the footprint matches the formula");
        // A facing end's radii are the uniform check's, over every corner of the outline.
        if (e.shape == DockShape::EdgeArc && std::isnan(at)) checkConcave(name, raster.outline, e, right);
        checkClearance(name, raster.outline, hull, points, e, right, at);
    };
    // Smaller tray's ramp against its contract: flat over the buttons up to where the S starts, the raster on the S by
    // columns where it is at most 45 degrees steep and by rows where it is steeper, the trace flat beyond its two tangent
    // points and on one of its circles or the run's line between them, so it spans dk + (rho + c) tau, and straight at
    // the Curve angle through the run's middle for the contract's run. The mod's top and foot fillets are SlantRounding's
    // two radii. Then its clearance: every traced point of the S keeps kEndSpace from the last button's plate within
    // 0.05, and is within 0.1 of it unless the S sits at its limit, and none reaches the first tray item's highlight.
    auto checkRamp = [&](char const* name, OutlineRaster const& raster, DockHull const& hull,
                         std::vector<std::array<float, 2>> const& points, SlantedRamp const& p, float band) {
        int through = int(p.d + (band - p.d) / 2);
        float flat = 0, profile = 0;
        for (int c = int(std::ceil(p.c1 - 40)); c + 1 <= p.c1; ++c) flat = std::max(flat, raster.farDepth(c, through));
        bool steep = p.angle > 45;
        // Each arc turns past 45 degrees this far along the hull from its tangent point.
        float bend = p.rho * std::sqrt(.5f), footBend = p.foot * std::sqrt(.5f);
        for (int c = int(std::floor(p.c1)) - 2; c <= int(std::ceil(p.c2)) + 2; ++c) {
            if (steep && c > p.c1 + bend - 1 && c < p.c2 - footBend) continue;
            profile = std::max(profile, std::abs(raster.farDepth(c, through) - SlantedEnd::column([&](float px) { return p.depth(px); }, float(c))));
        }
        // The S's rows steeper than 45 degrees: covered before it, open after it, so its x is a right wall's.
        float rowProfile = 0, rise = p.rho * (1 - std::sqrt(.5f)), footRise = p.foot * (1 - std::sqrt(.5f));
        int rows = 0;
        if (steep)
            for (int v = int(std::ceil(rise)); v + 1 <= p.d - footRise; ++v, ++rows) {
                float contract = SlantedEnd::column([&](float depth) { return p.wall(depth); }, float(v));
                rowProfile = std::max(rowProfile, std::abs(raster.rightWall(v, int(std::floor(contract)) - 6, int(std::ceil(contract)) + 6) - contract));
            }
        float circleBudget = geometryBudget + p.foot * cubicError(p.angle);
        float flatOff = 0, curveOff = 0, buttonLeast = 1e30f, trayLeast = 1e30f, iconLeast = 1e30f;
        auto trayPlate = EndPlate::tray(band, p.d);
        trayPlate.inset = 0;
        size_t curvePoints = 0;
        for (auto [px, v] : points) {
            if (px < p.c1 - 20 || px > p.c2 + 20 || v > through) continue;
            if (px >= p.c1) {
                buttonLeast = std::min(buttonLeast, p.plate.distance(px - p.gapStart, v));
                trayLeast = std::min(trayLeast, trayPlate.distance(p.trayLeft - px, v));
                if (p.icon) iconLeast = std::min(iconLeast, p.icon->distance(px - p.gapStart, v));
            }
            if (px <= p.c1 || px >= p.c2) { flatOff = std::max(flatOff, std::abs(v - (px <= p.c1 ? 0 : p.d))); continue; }
            float off = std::min({std::abs(std::hypot(px - p.c1, v - p.rho) - p.rho), std::abs(std::hypot(px - p.c2, v - (p.d - p.foot)) - p.foot),
                                  std::abs(px - p.runFromX() - p.k * (v - p.runFromV())) / p.h});
            curveOff = std::max(curveOff, off);
            ++curvePoints;
        }
        auto [tangent, run] = steepness(raster.outline, hull, points, p.midX(), p.midV());
        auto const& ramp = raster.outline.polygons.front();
        float top = ramp[1].radius, foot = ramp[2].radius;
        bool limited = p.shift >= p.limit - 1e-3f;
        printf("SLANTED-RAMP %s rho %.2f/%.2f (mod %.3f/%.3f) from %.3f to %.3f, moved %.3f DIP (limit %.3f; gap starts %.3f), span %.3f "
               "(dk + (rho + c) tau %.3f): over the buttons %.3f px; S raster worst %.3f px; trace off its flats %.4f DIP and its circles "
               "and line %.4f DIP over %zu points; straight run %.3f DIP (contract %.3f) at %.3f deg; nearest %.3f DIP from the last "
               "button's plate, %.3f from the first tray item's\n", name, p.rho, p.foot, top, foot, p.c1, p.c2, p.shift, p.limit,
               p.gapStart, p.c2 - p.c1, p.d * p.k + (p.rho + p.foot) * p.tau, flat, profile, flatOff, curveOff, curvePoints, run, p.run(),
               tangent, buttonLeast, trayLeast);
        if (rows)
            printf("SLANTED-RAMP-ROWS %s: %d rows steeper than 45 degrees, wall worst %.3f px; circles budget %.4f DIP\n", name, rows,
                   rowProfile, circleBudget);
        expect(flat <= .05f, name, "ramp: flat over the buttons up to where the S starts");
        expect(profile <= arcBudget && rowProfile <= arcBudget && flatOff <= geometryBudget, name,
               "ramp: the raster follows the contract's S over its span");
        expect(curvePoints > 0 && curveOff <= circleBudget, name,
               "ramp: every point between the depths on one of its circles at min(r, cap) or the run's line");
        expect(p.run() <= .1f || (std::abs(tangent - p.angle) <= .5f && std::abs(run - p.run()) <= .1f), name,
               "ramp: the straight run is the contract's length, at the Curve angle where there is one");
        expect(std::abs(foot - p.foot) <= .01f && std::abs(top - p.rho) <= .01f, name,
               "ramp: the top fillet keeps the convex radius as far as the step holds it and the foot takes the rest up to the "
               "concave radius");
        if (p.icon && iconLeast < 1e29f) printf("SLANTED-RAMP-ICON %s: %.3f DIP beside the last button's icon (contract %.3f)\n", name, iconLeast, p.iconGap());
        expect(buttonLeast >= kEndSpace - .05f && iconLeast >= kIconSpace - .1f &&
                   (limited || buttonLeast <= kEndSpace + .1f || iconLeast <= kIconSpace + .1f), name,
               "ramp: it moves toward the buttons until it is the end space from the last button's plate or kIconSpace beside its icon");
        expect(trayLeast >= -.05f, name, "ramp: it stays clear of the first tray item's highlight");
    };
    for (float angle : angles)
    for (bool atTop : {false, true}) {
        char text[96];
        std::string label;
        auto named = [&] { label = tagged(text, angle); return label.c_str(); };
        // Both ends of the band 80 matrix with Smaller tray 8, round ends on band 40, the user's band 68 at r 24 (their
        // Smaller tray 24 too), and r 60 there, past half the band, where Edge arc's cap and the ramp's bind and Edge keeps
        // a long run. Then Concave radius apart from 1.5 r (0.4.9): Separate Edge arc with inner arcs at r 24, Smaller tray
        // 20 and a concave 36; a concave far under the convex; one far over it; and the user's 24/50 with Smaller tray 24
        // on band 72 in both layouts, which round alike, Unified's short step giving its foot what the convex leaves.
        struct End { DockShape shape; DockLayout layout; float radius, drop, band; bool arcs = false; float concave = kDefaultConcave; };
        std::vector<End> ends;
        for (auto layout : {DockLayout::Unified, DockLayout::Separate})
            for (auto shape : {DockShape::Island, DockShape::Edge, DockShape::EdgeArc}) ends.push_back({shape, layout, 12, 8, 80});
        ends.push_back({DockShape::EdgeArc, DockLayout::Separate, 12, 8, 80, true});
        for (auto shape : {DockShape::Island, DockShape::EdgeArc}) ends.push_back({shape, DockLayout::Unified, 20, 16, 40});
        for (auto shape : {DockShape::Edge, DockShape::EdgeArc}) ends.push_back({shape, DockLayout::Unified, 24, 0, 68});
        ends.push_back({DockShape::EdgeArc, DockLayout::Unified, 24, 20, 68});
        ends.push_back({DockShape::EdgeArc, DockLayout::Unified, 24, 24, 68});
        ends.push_back({DockShape::Island, DockLayout::Unified, 24, 44, 68});
        ends.push_back({DockShape::EdgeArc, DockLayout::Unified, 60, 20, 68});
        ends.push_back({DockShape::Edge, DockLayout::Unified, 60, 0, 68});
        ends.push_back({DockShape::EdgeArc, DockLayout::Separate, 24, 20, 68, true, 36});
        ends.push_back({DockShape::EdgeArc, DockLayout::Unified, 24, 20, 68, false, 4});
        ends.push_back({DockShape::EdgeArc, DockLayout::Unified, 12, 8, 80, false, 40});
        ends.push_back({DockShape::EdgeArc, DockLayout::Separate, 24, 24, 72, true, 50});
        ends.push_back({DockShape::EdgeArc, DockLayout::Unified, 24, 24, 72, false, 50});
        // The drop past half the band: the tray wall holds less concave than the step, whose foot answers to itself.
        ends.push_back({DockShape::EdgeArc, DockLayout::Unified, 24, 40, 72, false, 50});
        for (auto const& c : ends) {
            if (!runs(angle, c.shape, c.layout, c.drop)) continue;
            bool separate = c.layout == DockLayout::Separate;
            snprintf(text, sizeof text, "%s-%s%s-r%g-c%g-d%g-band%g-%s", separate ? "separate" : "unified", shapeName(c.shape),
                     c.arcs ? "-inner-arcs" : "", c.radius, c.concave, c.drop, c.band, side(atTop));
            auto name = named();
            auto outline = MakeOutline(slanted({.shape = c.shape, .layout = c.layout, .innerArcs = c.arcs, .trayReduction = c.drop,
                                                .concave = c.concave}, angle),
                                       240, c.radius, c.band, atTop, kContent);
            auto hull = slantFit(c.shape, atTop, c.band);
            if (separate) hull = separateFit(hull, outline);
            OutlineRaster raster(outline, hull, width, height);
            auto points = traced(raster.outline, hull);
            auto radii = SlantRounding(c.shape, c.band, c.drop, c.radius, c.concave, angle);
            checkEnd(name, raster, hull, points, SlantedEnd(c.shape, c.band, 0, c.radius, radii, angle, EndPlate::buttons(c.band)), false);
            checkEnd(name, raster, hull, points,
                     SlantedEnd(c.shape, c.band, c.drop, c.radius, radii, angle, EndPlate::tray(c.band, c.drop)), true);
            if (!separate && c.drop > 0) checkRamp(name, raster, hull, points, SlantedRamp(hull, c.drop, 240, angle, radii.convex, c.concave, c.band), c.band);
            // Every convex curve rounds alike and every concave one alike (0.4.7: the step's S was far tighter than the
            // ends'), Separate's facing ends too; Unified's step, the first polygon's corners 1 and 2, is checkRamp's. A
            // Concave radius 20 DIP larger leaves every convex curve as it was (0.4.9: the user found each slider moving
            // both), and the other layout rounds at the same radii (0.4.9: the user's rule).
            auto variant = [&](DockLayout layout, float concaveRadius) {
                return MakeOutline(slanted({.shape = c.shape, .layout = layout, .innerArcs = c.arcs, .trayReduction = c.drop,
                                            .concave = concaveRadius}, angle),
                                   240, c.radius, c.band, atTop, kContent);
            };
            auto radiiOf = [&](DockOutline const& o, bool unified) {
                std::array<float, 2> found{-1, -1};
                for (size_t p = 0; p < o.polygons.size(); ++p) {
                    auto fillets = Fillets(o.polygons[p]);
                    for (size_t i = 0; i < o.polygons[p].size(); ++i) {
                        if (unified && c.drop > 0 && p == 0 && (i == 1 || i == 2)) continue;
                        if (o.polygons[p][i].radius > 0) found[fillets[i].sweep > 0 ? 0 : 1] = o.polygons[p][i].radius;
                    }
                }
                return found;
            };
            bool alike = true;
            for (size_t p = 0; p < raster.outline.polygons.size(); ++p) {
                auto const& polygon = raster.outline.polygons[p];
                auto fillets = Fillets(polygon);
                for (size_t i = 0; i < polygon.size(); ++i)
                    if (polygon[i].radius > 0 && !(!separate && c.drop > 0 && p == 0 && (i == 1 || i == 2)))
                        alike &= std::abs(polygon[i].radius - (fillets[i].sweep > 0 ? radii.convex : radii.concave)) < 1e-3f;
            }
            auto mine = radiiOf(raster.outline, !separate);
            auto moved = radiiOf(variant(c.layout, c.concave + 20), !separate);
            auto other = radiiOf(variant(separate ? DockLayout::Unified : DockLayout::Separate, c.concave), separate);
            printf("SLANTED-RADII %s: convex %.3f concave %.3f (contract %.3f/%.3f, settings %g/%g); other layout %.3f/%.3f\n", name,
                   mine[0], mine[1], radii.convex, radii.concave, c.radius, c.concave, other[0], other[1]);
            expect(alike, name, "radii: every convex curve rounds at the corner radius as far as it allows and every concave one at "
                   "Concave radius up to the room left");
            expect(mine[0] < 0 || std::abs(moved[0] - mine[0]) < 1e-3f, name, "radii: a larger Concave radius leaves every convex curve as it was");
            expect(std::abs(other[0] - mine[0]) < 1e-3f && (other[1] < 0 || mine[1] < 0 || std::abs(other[1] - mine[1]) < 1e-3f), name,
                   "radii: Unified and Separate round at the same radii");
            if (!separate) continue;
            // Separate's facing ends are the outer ends' S, mirrored (0.4.8): the button bar's keeps its distance from the
            // last button's plate as Start's end from the first's, the tray bar's from the first tray item's highlight,
            // and without inner arcs Edge arc's meets the screen edge sharp as Edge's does. Each starts at its bar's edge,
            // kDockPadding from its content, and the bars stand Bar distance (12 DIP) apart at the facing ends' nearest
            // points: the tray content sits that and both spans, padding and footprint, after the last button.
            auto facingShape = c.shape == DockShape::EdgeArc && !c.arcs ? DockShape::Edge : c.shape;
            SlantedEnd buttonsFacing(facingShape, c.band, 0, c.radius, radii, angle, EndPlate::buttons(c.band)),
                trayFacing(facingShape, c.band, c.drop, c.radius, radii, angle, EndPlate::tray(c.band, c.drop));
            float gap = 12 + (8 + buttonsFacing.footprint()) + (8 + trayFacing.footprint());
            printf("SLANTED-FACING %s: tray %.3f DIP after the last button (contract %.3f)\n", name, raster.outline.gap, gap);
            expect(std::abs(float(raster.outline.gap) - gap) <= .06f, name,
                   "facing: the tray sits Bar distance and both facing spans after the last button");
            // The S checks read each end against its own edge, which the mod's gap places: the gap is checked above.
            checkEnd(name, raster, hull, points, buttonsFacing, true, hull.right - (240 + float(raster.outline.gap)));
            checkEnd(name, raster, hull, points, trayFacing, false, hull.right - (240 + 16));
        }
        // Clearance with Smaller tray off, inner arcs too.
        for (auto layout : {DockLayout::Unified, DockLayout::Separate})
            for (auto shape : {DockShape::Island, DockShape::Edge, DockShape::EdgeArc})
                for (bool arcs : {false, true}) {
                    if (arcs && (layout == DockLayout::Unified || shape != DockShape::EdgeArc)) continue;
                    if (!runs(angle, shape, layout, 0)) continue;
                    snprintf(text, sizeof text, "%s-%s%s-%s", layout == DockLayout::Separate ? "separate" : "unified", shapeName(shape),
                             arcs ? "-inner-arcs" : "", side(atTop));
                    auto name = named();
                    auto outline = MakeOutline(slanted({.shape = shape, .layout = layout, .innerArcs = arcs}, angle), 240, 12, 80, atTop, kContent);
                    auto hull = slantFit(shape, atTop, 80);
                    if (layout == DockLayout::Separate) hull = separateFit(hull, outline);
                    auto points = traced(outline, hull);
                    auto rounding = SlantRounding(shape, 80, 0, 12, kDefaultConcave, angle);
                    checkClearance(name, outline, hull, points, SlantedEnd(shape, 80, 0, 12, rounding, angle, EndPlate::buttons(80)), false);
                    checkClearance(name, outline, hull, points, SlantedEnd(shape, 80, 0, 12, rounding, angle, EndPlate::tray(80, 0)), true);
                }
        // Every end with an icon (0.4.9, the user's pick): it moves in or out until it leaves kIconSpace beside it, the
        // left end beside Start's, the right end beside the last tray icon, out too when that icon sits at the tray's end,
        // and Separate's facing ends beside the last button's and the first tray icon, from their bars' edges. An end
        // without its icon keeps its plate, whatever the other icons, and Unified's step stops kIconSpace beside the last
        // button's icon or the end space from its plate. Beside each icon's top row the walls stand the same distance
        // off; the least distances over the whole end are reported too, which with Smaller tray the far fillet above the
        // tray icon sets.
        for (auto const& c : ends)
        for (float trayInset : {28.f, 0.f}) {
            if (!runs(angle, c.shape, c.layout, c.drop)) continue;
            bool separate = c.layout == DockLayout::Separate;
            snprintf(text, sizeof text, "mirrored-%s-%s%s-r%g-c%g-d%g-band%g-icon%g-%s", separate ? "separate" : "unified",
                     shapeName(c.shape), c.arcs ? "-inner-arcs" : "", c.radius, c.concave, c.drop, c.band, trayInset,
                     side(atTop));
            auto name = named();
            auto style = slanted({.shape = c.shape, .layout = c.layout, .innerArcs = c.arcs, .trayReduction = c.drop,
                                   .concave = c.concave}, angle);
            auto content = Mirrored(c.band, c.drop, trayInset), buttonOnly = kContent, trayOnly = kContent;
            buttonOnly.buttonIcon = content.buttonIcon;
            trayOnly.trayIcon = content.trayIcon;
            auto plain = MakeOutline(style, 240, c.radius, c.band, atTop, kContent);
            auto mirroredOutline = MakeOutline(style, 240, c.radius, c.band, atTop, content);
            auto hull = slantFit(c.shape, atTop, c.band);
            if (separate) hull = separateFit(hull, mirroredOutline);
            OutlineRaster raster(mirroredOutline, hull, width, height);
            // The corners on one hull edge: Start's end on the left, everything from the buttons' end on the right.
            auto on = [](DockOutline const& o, HullEdge edge) {
                std::vector<DockCorner> found;
                for (auto const& polygon : o.polygons)
                    for (auto const& corner : polygon)
                        if (corner.x.edge == edge) found.push_back(corner);
                return found;
            };
            expect(on(MakeOutline(style, 240, c.radius, c.band, atTop, buttonOnly), HullEdge::Right) == on(plain, HullEdge::Right) &&
                   on(MakeOutline(style, 240, c.radius, c.band, atTop, trayOnly), HullEdge::Left) == on(plain, HullEdge::Left), name,
                   "icons: an end without its icon keeps its plate, whatever the other icons");
            auto points = traced(raster.outline, hull);
            auto rounding = SlantRounding(c.shape, c.band, c.drop, c.radius, c.concave, angle);
            SlantedEnd left(c.shape, c.band, 0, c.radius, rounding, angle, EndPlate::icon(*content.buttonIcon), kIconSpace),
                right(c.shape, c.band, c.drop, c.radius, rounding, angle, EndPlate::icon(*content.trayIcon), kIconSpace);
            movedOut += trayInset == 0 && right.offset < 0;
            auto leftIcon = left.plate, rightIcon = right.plate, leftAll = leftIcon, rightAll = rightIcon;
            leftAll.beside = rightAll.beside = false;
            float leftLeast = nearestTo(hull, points, left, false, leftIcon), rightLeast = nearestTo(hull, points, right, true, rightIcon);
            printf("SLANTED-ICONS %s: left end in %.3f DIP, %.3f DIP beside the first button's icon (contract %.3f, %.3f at its top row, "
                   "least %.3f); right end in %.3f DIP (%.3f by the tray plate), %.3f DIP beside the last tray icon (%.3f at its top row, "
                   "least %.3f)\n", name, left.offset, leftLeast, kIconSpace, left.wall(leftIcon.top) + leftIcon.inset,
                   nearestTo(hull, points, left, false, leftAll), right.offset,
                   SlantedEnd(c.shape, c.band, c.drop, c.radius, rounding, angle, EndPlate::tray(c.band, c.drop)).offset, rightLeast,
                   right.wall(rightIcon.top) + rightIcon.inset, nearestTo(hull, points, right, true, rightAll));
            expect(std::abs(leftLeast - kIconSpace) <= .1f && std::abs(rightLeast - kIconSpace) <= .1f, name,
                   "icons: the outer ends leave kIconSpace beside Start's icon and the last tray icon");
            checkEnd(name, raster, hull, points, left, false);
            checkEnd(name, raster, hull, points, right, true);
            if (!separate) {
                if (c.drop > 0)
                    checkRamp(name, raster, hull, points, SlantedRamp(hull, c.drop, 240, angle, rounding.convex, c.concave, c.band, content.lastButtonIcon),
                              c.band);
                continue;
            }
            // Separate's facing ends (0.4.8) beside the last button's icon and the first tray icon, each from its bar's
            // edge, out as well as in, and the tray sits Bar distance and both facing spans after the buttons.
            auto facingShape = c.shape == DockShape::EdgeArc && !c.arcs ? DockShape::Edge : c.shape;
            SlantedEnd buttonsFacing(facingShape, c.band, 0, c.radius, rounding, angle, EndPlate::icon(*content.lastButtonIcon),
                                     kIconSpace),
                trayFacing(facingShape, c.band, c.drop, c.radius, rounding, angle, EndPlate::icon(*content.firstTrayIcon),
                           kIconSpace);
            float gap = 12 + (8 + buttonsFacing.footprint()) + (8 + trayFacing.footprint());
            float buttonsEdge = hull.right - (240 + float(raster.outline.gap)), trayEdge = hull.right - 256;
            float buttonsLeast = nearestTo(hull, points, buttonsFacing, true, buttonsFacing.plate, buttonsEdge),
                  trayLeast = nearestTo(hull, points, trayFacing, false, trayFacing.plate, trayEdge);
            printf("SLANTED-ICONS-FACING %s: tray %.3f DIP after the last button (contract %.3f); %.3f DIP beside the last button's "
                   "icon, %.3f beside the first tray icon (Start's %.3f)\n", name, raster.outline.gap, gap, buttonsLeast,
                   trayLeast, leftLeast);
            expect(std::abs(float(raster.outline.gap) - gap) <= .06f, name,
                   "icons: the tray sits Bar distance and both facing spans after the last button");
            expect(std::abs(buttonsLeast - kIconSpace) <= .1f && std::abs(trayLeast - kIconSpace) <= .1f, name,
                   "icons: the facing ends leave kIconSpace beside their icons");
            checkEnd(name, raster, hull, points, buttonsFacing, true, buttonsEdge);
            checkEnd(name, raster, hull, points, trayFacing, false, trayEdge);
        }
    }
    // How far each end moves in and reaches past the hull on screen per Curve angle, on the user's band 68 at r 24 and on
    // a 48 DIP taskbar's band 40 at r 12, there also at the slider's widest 30: the trace's outermost point against the
    // contract's footprint, beside the mod's OutlineReach, which sizes the preview's margin and may only overshoot (an
    // Island's toe vertex).
    for (auto [angle, band, radius] : {std::array<float, 3>{45, 68, 24}, {60, 68, 24}, {70, 68, 24}, {85, 68, 24}, {70, 40, 12},
                                       {30, 40, 12}})
        for (auto shape : {DockShape::Island, DockShape::Edge, DockShape::EdgeArc}) {
            char name[96];
            snprintf(name, sizeof name, "unified-%s-r%g-band%g-angle%g", shapeName(shape), radius, band, angle);
            auto hull = slantFit(shape, false, band);
            auto outline = MakeOutline(slanted({.shape = shape}, angle), 240, radius, band, false, kContent);
            float reach[2]{-1e9f, -1e9f}, mod = float(OutlineReach(outline, band));
            for (auto [x, v] : traced(outline, hull))
                if (v <= band + 1e-3f) reach[0] = std::max(reach[0], hull.left - x), reach[1] = std::max(reach[1], x - hull.right);
            auto rounding = SlantRounding(shape, band, 0, radius, kDefaultConcave, angle);
            SlantedEnd left(shape, band, 0, radius, rounding, angle, EndPlate::buttons(band)),
                right(shape, band, 0, radius, rounding, angle, EndPlate::tray(band, 0));
            printf("SLANTED-REACH %s: left end in %.3f DIP, past the hull %.3f DIP (contract %.3f); right end in %.3f DIP, past the hull "
                   "%.3f DIP (contract %.3f); OutlineReach %.3f\n", name, left.offset, reach[0], left.footprint(), right.offset, reach[1],
                   right.footprint(), mod);
            float widest = std::max(reach[0], reach[1]);
            expect(std::abs(reach[0] - left.footprint()) <= .06f && std::abs(reach[1] - right.footprint()) <= .06f && mod >= widest - .06f &&
                   (shape == DockShape::Island || std::abs(mod - widest) <= .06f), name, "reach: the footprint matches the formula");
        }
    // The tray's end where the dock doesn't fit its curve (0.4.8): an upright, sharp wall `square` past the hull's right
    // edge and so past the screen's side, the rest of the outline as it was. Upright outlines move their last piece's
    // right side there and drop its flare.
    for (auto layout : {DockLayout::Unified, DockLayout::Separate}) {
        auto style = slanted({.shape = DockShape::EdgeArc, .layout = layout, .trayReduction = 8}, 70);
        auto curved = MakeOutline(style, 240, 12, 80, false, kContent), square = MakeOutline(style, 240, 12, 80, false, kContent, 30);
        auto const& end = square.polygons.back();
        int sharp = 0;
        for (auto const& corner : end) sharp += corner.x.edge == HullEdge::Right && corner.x.offset == 30 && corner.radius == 0;
        bool kept = square.polygons.front()[0] == curved.polygons.front()[0] && square.gap == curved.gap;
        printf("SLANTED-SQUARE %s: %d sharp corners at 30, reach %.3f (curved %.3f), rest kept %d\n",
               layout == DockLayout::Separate ? "separate" : "unified", sharp, square.reach, curved.reach, int(kept));
        expect(sharp == 2 && square.reach == 30 && curved.reach > 0 && kept, layout == DockLayout::Separate ? "square-separate" : "square-unified",
               "square: the tray's end is an upright wall past the screen's side, the rest unchanged");
    }
    auto upright = MakeOutline({.shape = DockShape::EdgeArc}, 240, 12, 80, false, kContent, 30);
    expect(upright.pieces.back().right.edge == HullEdge::Right && upright.pieces.back().right.offset == 30 &&
           upright.pieces.back().flareRight == 0 && upright.reach == 30 && MakeOutline({.shape = DockShape::EdgeArc}, 240, 12, 80,
           false, kContent).reach == 12, "square-upright", "square: an upright outline's tray side runs past the screen's side unflared");
    // Unless some icon-at-the-end case moves out, a wall that may only move in would pass them all.
    expect(movedOut > 0, "mirrored-icon0", "mirror: a last tray icon at the tray's end moves the right end out");
    printf("SLANTED-INTENT %d failures, %d ends moved out\n", failures, movedOut);
    assert(failures == 0);
    puts("PASS: slanted ends curve each outer wall at 45, 70 and 85 degrees: the far side is flat up to the far fillet, every "
         "point from there to the screen edge lies on the far or the foot circle, every convex curve of the outline at the "
         "corner radius as far as each curve allows and every concave one at Concave radius up to the room left, so a larger "
         "Concave radius leaves the convex curves as they were, and both layouts round alike (r 12, 20, 24 and 60 with "
         "Concave radius 18, and 24/36, 24/4, 12/40 and 24/50 in both layouts), or on "
         "the straight run, which is the contract's length at the Curve angle, Edge arc's flare takes the concave radius, the "
         "footprint matches the formula, Separate's facing ends are the outer ends' S mirrored, Bar distance apart where "
         "nearest, and each wall moves in or out until it leaves 24 DIP beside its icon, or without one until it is the 6 DIP "
         "end space from its content plate; Smaller tray's step is an S of a convex arc at the convex radius as far as the "
         "step holds it, a straight run at the Curve angle and a concave arc of the rest up to Concave radius, moved toward the "
         "buttons until it is the end space from the last button's plate or 24 DIP beside its icon");
    puts("PASS: slant off packs no polygon over the whole matrix, slant on at 45 and 70 degrees packs (2, bars, side, 0) and "
         "the corner counts, and Unified Island's far corner and toe pack the contract's values; ReadDockStyle leans the walls "
         "90 less the Curve angle clamped to 30..85, 20 degrees by default, and not at all with Slanted ends off");
    // Interior field over both ends, the ramp and a slanted Edge arc foot.
    // The user's band 68 Edge arc reaches furthest past each end, so its hull is narrower to keep both on the canvas.
    struct Interior { char const* name; DockStyle style; float radius, band, left, right; };
    for (float angle : angles)
    for (auto const& c : {Interior{"slanted-unified-island-r24-d44-band68", {.trayReduction = 44}, 24, 68, 100.25f, 420.25f},
                          Interior{"slanted-unified-edge-arc-r12-d8-band40", {.shape = DockShape::EdgeArc, .trayReduction = 8}, 12, 40,
                                   100.25f, 420.25f},
                          Interior{"slanted-separate-edge-arc-inner-arcs-r12-d8-band40",
                                   {.shape = DockShape::EdgeArc, .layout = DockLayout::Separate, .innerArcs = true, .trayReduction = 8}, 12, 40,
                                   100.25f, 420.25f},
                          Interior{"slanted-unified-edge-arc-r24-d20-band68", {.shape = DockShape::EdgeArc, .trayReduction = 20}, 24, 68,
                                   160.25f, 380.25f}}) {
        if (!runs(angle, c.style.shape, c.style.layout, c.style.trayReduction)) continue;
        for (bool atTop : {false, true}) {
            auto name = tagged(std::string(c.name) + "-" + side(atTop), angle);
            float edge = c.style.shape == DockShape::Island ? 4.f : 0.f, top = atTop ? edge : 88 - edge - c.band, d = float(TrayDrop(c.style, c.band));
            DockHull hull{c.left, top, c.right, top + c.band};
            auto rounding = SlantRounding(c.style.shape, c.band, d, c.radius, float(c.style.concave), angle);
            SlantedEnd left(c.style.shape, c.band, 0, c.radius, rounding, angle, EndPlate::buttons(c.band)),
                right(c.style.shape, c.band, d, c.radius, rounding, angle, EndPlate::tray(c.band, d));
            // Each end's window runs from 10 DIP past its footprint to 30 DIP inside the hull, or 10 DIP in from where its
            // far side leaves the flat if that is further.
            std::vector<std::pair<float, float>> windows{{hull.left - left.footprint() - 10, hull.left + std::max(30.f, 10 - left.farStart())},
                                                         {hull.right - std::max(30.f, 10 - right.farStart()), hull.right + right.footprint() + 10}};
            SlantedRamp ramp(hull, d, 120, angle, rounding.convex, float(c.style.concave), c.band);
            auto outline = MakeOutline(slanted(c.style, angle), 120, c.radius, c.band, atTop, kContent);
            // Separate's facing ends, from the last button's edge to the tray bar's, the outline's gap apart.
            if (c.style.layout == DockLayout::Separate) windows.push_back({hull.right - 120 - float(outline.gap) - 30, hull.right - 136 + 30});
            else windows.push_back({ramp.c1 - 20, ramp.c2 + 20});
            CheckWindowedField(name.c_str(), outline, hull, windows);
        }
    }
    // Issue 07, measured and not asserted: the upright 0.4.3 Edge arc flare's field at its foot, and an 8 DIP border.
    {
        DockHull hull{100.25f, 48, 420.25f, 88};
        auto outline = MakeOutline({.shape = DockShape::EdgeArc}, 120, 12, 40, false, kContent);
        CheckWindowedField("upright-unified-edge-arc-r12-band40-bottom-flare-foot (issue 07)", outline, hull,
                           {{hull.left - 16, hull.left + 20}}, true);
        LiquidRenderer::LensConstants lens{};
        lens.imageSize[0] = 800; lens.imageSize[1] = 132;
        lens.center[0] = (hull.left + hull.right) / 2 * 1.5f; lens.center[1] = (hull.top + hull.bottom) / 2 * 1.5f;
        lens.halfSize[0] = (hull.right - hull.left) / 2 * 1.5f; lens.halfSize[1] = 20 * 1.5f;
        lens.radius = 18;
        ApplyDockShape(lens, outline, 1.5f);
        TraceDistance trace(outline, hull, 1.5f, {hull.left - 20, -20, hull.left + 30, 120});
        float x = (hull.left + 6) * 1.5f, y = 131.5f; // 6 DIP inside the wall on the screen-edge row.
        lens.outputOffset[0] = x - .5f; lens.outputOffset[1] = y - .5f;
        printf("ISSUE-07 upright flare foot: 6 DIP inside the wall on the screen-edge row the field reads %.3f px, the trace %.3f px\n",
               DockField(lens, 1, 1)[0][0], trace(x, y));
    }
    puts("PASS: around both slanted ends, the ramp and a slanted Edge arc foot, at 45 and 70 degrees and at 85 for the Island "
         "ends and the ramp, the shader's field and optical field are the trace's signed distance within half a pixel 12 DIP "
         "deep, and 1, 8 and 10 DIP border bands keep an even width");
    // Structure: the morph's two keyframe hulls, the second inverted, trace the same call kinds.
    struct Case { std::string name; DockStyle style; float radius, band; };
    std::vector<Case> shapes;
    for (auto layout : {DockLayout::Unified, DockLayout::Separate})
        for (auto shape : {DockShape::Island, DockShape::Edge, DockShape::EdgeArc})
            shapes.push_back({std::string("slanted-") + (layout == DockLayout::Separate ? "separate-" : "unified-") + shapeName(shape),
                              {.shape = shape, .layout = layout}, 12, 80});
    shapes.push_back({"slanted-unified-island-r20-band40", {}, 20, 40});
    shapes.push_back({"slanted-unified-edge-arc-r20-band40", {.shape = DockShape::EdgeArc}, 20, 40});
    shapes.push_back({"slanted-unified-island-r12-d8", {.trayReduction = 8}, 12, 80});
    shapes.push_back({"slanted-unified-island-r20-d8", {.trayReduction = 8}, 20, 80});
    shapes.push_back({"slanted-unified-edge-arc-r12-d8", {.shape = DockShape::EdgeArc, .trayReduction = 8}, 12, 80});
    shapes.push_back({"slanted-unified-edge-arc-r20-d8", {.shape = DockShape::EdgeArc, .trayReduction = 8}, 20, 80});
    shapes.push_back({"slanted-unified-island-r24-d44-band68", {.trayReduction = 44}, 24, 68});
    shapes.push_back({"slanted-unified-island-r4-d44-band68", {.trayReduction = 44}, 4, 68});
    shapes.push_back({"slanted-separate-island-r24-d44-band68", {.layout = DockLayout::Separate, .trayReduction = 44}, 24, 68});
    shapes.push_back({"slanted-separate-edge-arc-inner-arcs", {.shape = DockShape::EdgeArc, .layout = DockLayout::Separate, .innerArcs = true}, 12, 80});
    shapes.push_back({"slanted-separate-edge-arc-inner-arcs-d8",
                      {.shape = DockShape::EdgeArc, .layout = DockLayout::Separate, .innerArcs = true, .trayReduction = 8}, 12, 80});
    shapes.push_back({"slanted-unified-edge-arc-r24-band68", {.shape = DockShape::EdgeArc}, 24, 68});
    shapes.push_back({"slanted-unified-edge-arc-r24-d20-band68", {.shape = DockShape::EdgeArc, .trayReduction = 20}, 24, 68});
    std::vector<Case> cases;
    for (float angle : angles)
        for (auto const& c : shapes)
            if (runs(angle, c.style.shape, c.style.layout, c.style.trayReduction))
                cases.push_back({tagged(c.name, angle), slanted(c.style, angle), c.radius, c.band});
    for (float angle : angles) {
        int structures = 0;
        for (auto const& c : cases) {
            if (c.style.slant != 90 - angle) continue;
            for (bool atTop : {false, true}) {
                auto outline = MakeOutline(c.style, 150, c.radius, c.band, atTop, kContent);
                auto kinds = [&](float left) { CallKinds sink; TraceOutline(&sink, outline, {left, 4, 5120 - left, 76}); return sink.kinds; };
                auto from = kinds(-5120), to = kinds(10240);
                if (from != to)
                    printf("SLANTED-STRUCTURE %s-%s differs: %s at -5120, %s at 10240\n", c.name.c_str(), side(atTop), from.c_str(), to.c_str());
                assert(from == to && from == kinds(1000));
                ++structures;
            }
        }
        printf("SLANTED-STRUCTURE%s %d outlines trace identical call kinds at the hulls -5120 and 10240; Unified Island: %s\n",
               tagged("", angle).c_str(), structures,
               [&] { CallKinds sink; TraceOutline(&sink, MakeOutline(slanted({}, angle), 150, 12, 72, false, kContent), {0, 4, 640, 76}); return sink.kinds; }().c_str());
    }
    puts("PASS: every slanted outline at 45, 70 and 85 degrees traces the same call kinds at the morph's two keyframe hulls, "
         "the second inverted");
    // Parity on hulls with room for the feet.
    for (auto const& c : cases)
        for (bool atTop : {false, true}) {
            auto name = c.name + "-" + side(atTop);
            CheckOutlineParity(name.c_str(), MakeOutline(c.style, 240, c.radius, c.band, atTop, kContent), slantFit(c.style.shape, atTop, c.band), width, height);
        }
    puts("PASS: with slanted ends at 45 and 70 degrees, and at 85 for the Island ends and the ramp, the trace and the shader's "
         "polygon field agree over Unified/Separate x Island/Edge/Edge arc x top/bottom, with round ends at band 40, the ramp "
         "at d 8, 20 and 44, at band 68 at r 24 and r 4, and with inner arcs");
}

void RunOutlineParity() {
    LiquidRenderer::LensConstants lens{};
    ApplyDockShape(lens, MakeOutline({}, 240, 12, 80, false, kContent), 1.5f);
    assert(lens.dockShape[0] == 0); // Unified Island packs nothing: today's shader path.
    ApplyDockShape(lens, MakeOutline({.shape = DockShape::Edge}, 240, 12, 80, true, kContent), 1.5f);
    assert(lens.dockShape[0] == 1 && lens.dockShape[1] == 1 && lens.dockPieceRect[0][1] == -18 && lens.dockPieceRect[0][3] == 0 &&
           lens.dockPieceSide[0][0] == 0 && lens.dockPieceSide[0][1] == 1);
    lens = {};
    ApplyDockShape(lens, MakeOutline({.layout = DockLayout::Separate}, 240, 12, 80, false, kContent), 1.5f);
    // The button bar ends 12 DIP before the tray bar; both of the tray bar's edges hang off the hull's right edge.
    assert(lens.dockShape[1] == 2 && lens.dockPieceSide[0][0] == 0 && lens.dockPieceSide[0][1] == 1 &&
           lens.dockPieceRect[0][2] == -(240 + 16 + 12) * 1.5f && lens.dockPieceSide[1][0] == 1 && lens.dockPieceSide[1][1] == 1 &&
           lens.dockPieceRect[1][0] == -(240 + 16) * 1.5f && lens.dockPieceRect[1][2] == 0);
    // Only flares carry the screen edge's side and their radii; Edge and Separate pack exactly as before them.
    lens = {};
    ApplyDockShape(lens, MakeOutline({.shape = DockShape::Edge, .layout = DockLayout::Separate}, 240, 12, 80, false, kContent), 1.5f);
    assert(lens.dockShape[2] == 0 && lens.dockPieceSide[0][2] == 0 && lens.dockPieceSide[0][3] == 0 &&
           lens.dockPieceSide[1][2] == 0 && lens.dockPieceSide[1][3] == 0);
    lens = {};
    ApplyDockShape(lens, MakeOutline({.shape = DockShape::EdgeArc}, 240, 12, 80, true, kContent), 1.5f);
    assert(lens.dockShape[1] == 1 && lens.dockShape[2] == -1 && lens.dockPieceRect[0][1] == -18 &&
           lens.dockPieceSide[0][2] == 18 && lens.dockPieceSide[0][3] == 18);
    lens = {};
    // Facing walls stay straight unless inner arcs are on, then flare with the outer arcs' radius, and the bars part by
    // both arcs so their tips stay Bar distance apart.
    ApplyDockShape(lens, MakeOutline({.shape = DockShape::EdgeArc, .layout = DockLayout::Separate}, 240, 12, 80, false, kContent), 1.5f);
    assert(lens.dockShape[2] == 1 && lens.dockPieceSide[0][2] == 18 && lens.dockPieceSide[0][3] == 0 &&
           lens.dockPieceSide[1][2] == 0 && lens.dockPieceSide[1][3] == 18);
    ApplyDockShape(lens, MakeOutline({.shape = DockShape::EdgeArc, .layout = DockLayout::Separate, .innerArcs = true}, 240, 12, 80, false, kContent), 1.5f);
    assert(lens.dockPieceSide[0][3] == 18 && lens.dockPieceSide[1][2] == 18 &&
           lens.dockPieceRect[0][2] == -(240 + 16 + 12 + 2 * 12) * 1.5f && lens.dockPieceRect[1][0] == -(240 + 16) * 1.5f);
    // Unified Edge arc is one piece but still a path; Edge, and Edge arc at radius 0, keep the extended rounded rect.
    assert(NeedsPath(MakeOutline({.shape = DockShape::EdgeArc}, 240, 12, 80, false, kContent)) &&
           !NeedsPath(MakeOutline({.shape = DockShape::Edge}, 240, 12, 80, false, kContent)) &&
           !NeedsPath(MakeOutline({.shape = DockShape::EdgeArc}, 240, 0, 80, false, kContent)) &&
           NeedsPath(MakeOutline({.layout = DockLayout::Separate}, 240, 12, 80, false, kContent)));
    UINT width = 640, height = 420;
    auto hullFor = [&](DockShape shape, bool atTop) {
        float band = 80, edge = shape == DockShape::Island ? 4.f : 0.f;
        float top = atTop ? edge : height - edge - band;
        return DockHull{48.25f, top, 48.25f + 604, top + band}; // A quarter-pixel edge exercises the rim.
    };
    for (auto layout : {DockLayout::Unified, DockLayout::Separate})
        for (auto shape : {DockShape::Island, DockShape::Edge, DockShape::EdgeArc})
            for (bool atTop : {false, true}) {
                char name[64];
                snprintf(name, sizeof name, "%s-%s-%s", layout == DockLayout::Separate ? "separate" : "unified",
                         shape == DockShape::Island ? "island" : shape == DockShape::Edge ? "edge" : "edge-arc",
                         atTop ? "top" : "bottom");
                CheckOutlineParity(name, MakeOutline({.shape = shape, .layout = layout}, 240, 12, 80, atTop, kContent),
                                   hullFor(shape, atTop), width, height);
            }
    DockStyle arced{.shape = DockShape::EdgeArc, .layout = DockLayout::Separate, .innerArcs = true};
    for (bool atTop : {false, true})
        CheckOutlineParity(atTop ? "separate-edge-arc-inner-arcs-top" : "separate-edge-arc-inner-arcs-bottom",
                           MakeOutline(arced, 240, 12, 80, atTop, kContent), hullFor(DockShape::EdgeArc, atTop), width, height);
    puts("PASS: the outline trace and the shader's piece distance agree over Unified/Separate x Island/Edge/Edge arc x "
         "top/bottom and inner arcs, Unified Island keeps today's shader path, and only flares pack their radii");
    // The compositor's coverage on the screen-edge row, where the flares meet the edge. The hull's left wall is at
    // 48.25 (its right runs past the canvas); Separate's bar gap runs from 384.25 to 396.25, and with inner arcs, 12 DIP
    // each, the walls stand 52 DIP apart from 360.25 so the arcs' tips, at 372.25 and 384.25, are Bar distance apart.
    for (bool atTop : {false, true}) {
        auto hull = hullFor(DockShape::EdgeArc, atTop);
        UINT edgeRow = atTop ? 0 : height - 1;
        float discRow = atTop ? 12 : height - 13;
        auto coverage = [&](DockStyle style) { return TraceCoverage(MakeOutline(style, 240, 12, 80, atTop, kContent), hull, width, height); };
        auto unified = coverage({.shape = DockShape::EdgeArc});
        auto at = [&](std::vector<float> const& c, float x, float y) { return c[size_t(y) * width + size_t(x)]; };
        // An outer flare's foot is covered half its radius outside the wall; the disc it curves around is not.
        assert(at(unified, 42.25f, edgeRow) == 1 && at(unified, 36.25f, discRow) == 0);
        auto apart = coverage({.shape = DockShape::EdgeArc, .layout = DockLayout::Separate});
        // Apart, the facing walls meet the edge straight: the gap stays open down to the edge.
        for (float x = 385; x < 396; ++x) assert(at(apart, x, edgeRow) == 0);
        // With inner arcs each facing flare covers the edge row from its wall to 4 px short of its tip, where the arc
        // thins below half a pixel, and the row is open between the tips (measured: full to 366, 0 from 372 to 383).
        auto arcs = coverage(arced);
        for (float x = 362; x < 368; ++x) assert(at(arcs, x, edgeRow) >= .5f);
        for (float x = 388; x < 395; ++x) assert(at(arcs, x, edgeRow) >= .5f);
        for (float x = 373; x < 384; ++x) assert(at(arcs, x, edgeRow) == 0);
    }
    puts("PASS: on the screen-edge row an outer flare covers its foot and not its disc, the gap between apart bars is "
         "open, and inner arcs, the outer ones' radius, cover the row up to their tips, which stand Bar distance apart");
    RunSmallerTrayOutline(width, height);
    RunSlantedOutline();
}

void RunSimpleJob(CatalogIconJob& job) {
    job.thread = CreateThread(nullptr, 0, CatalogIconWorker, &job, 0, nullptr);
    assert(job.thread);
    assert(WaitForSingleObject(job.thread, 30000) == WAIT_OBJECT_0);
    CloseHandle(job.thread); job.thread = nullptr;
    if (!job.error.empty()) fwprintf(stderr, L"Simple Icons job failed: %s\n", job.error.c_str());
    assert(job.done && job.error.empty());
}

FrameworkElement FindTestElement(FrameworkElement const& element, PCWSTR name) {
    if (!element) return nullptr;
    if (element.Name() == name) return element;
    for (int i = 0; i < Media::VisualTreeHelper::GetChildrenCount(element); ++i) {
        auto found = FindTestElement(Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>(), name);
        if (found) return found;
    }
    return nullptr;
}

// The editor's lenses in window pixels, the window output's first. The content layer holds its own in the
// scrolled content's pixels, so they move by that content's origin in the window.
struct EditorLens { float native[4], clip[4], radius, alpha, kind; bool layer; };
std::vector<EditorLens> EditorLenses(SettingsEditor& ui) {
    float scale = GetDpiForWindow(ui.window) / 96.0f;
    auto origin = ui.contentLayer.TransformToVisual(ui.content).TransformPoint({0, 0});
    std::vector<EditorLens> lenses;
    std::lock_guard lock(ui.backgroundRenderer->mutex);
    for (bool layer : {false, true}) {
        auto const& lens = layer ? ui.backgroundRenderer->layerLens : ui.backgroundRenderer->lens;
        float dx = layer ? origin.X * scale : 0, dy = layer ? origin.Y * scale : 0;
        for (uint32_t i = 0; i < lens.lensCount; ++i) {
            auto const* native = lens.lensNativeRects[i];
            auto const* clip = lens.lensRects[i];
            lenses.push_back({{native[0] + dx, native[1] + dy, native[2], native[3]}, {clip[0] + dx, clip[1] + dy, clip[2], clip[3]},
                              lens.lensRadii[i][0], lens.lensTints[i][3], lens.lensShapeParams[i][3], layer});
        }
    }
    return lenses;
}

void PumpEditorFrames() {
    auto until = GetTickCount64() + 120;
    do {
        MSG message{};
        while (GetTickCount64() < until && PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        Sleep(5);
    } while (GetTickCount64() < until);
}

void PinEditorToPrimary();
// The window manager here can tile the editor again in the middle of a check (1501x2030 at (6171, 122), on the second
// monitor, was seen), and a read then sees another layout. While ShownEditor holds a pin, each pixel read first puts
// the editor back on it.
RECT g_editorPin{};
void HoldEditorPin(SettingsEditor& editor) {
    RECT now{};
    if (IsRectEmpty(&g_editorPin) || !GetWindowRect(editor.window, &now) || EqualRect(&now, &g_editorPin)) return;
    fprintf(stderr, "EDITOR-REPINNED before a pixel read: %ldx%ld at (%ld, %ld)\n", now.right - now.left, now.bottom - now.top,
            now.left, now.top);
    fflush(stderr);
    PinEditorToPrimary();
    GetWindowRect(editor.window, &g_editorPin);
    // The content takes the window's size and DPI some messages later, and a read before then maps its point through
    // the tile's layout: one fell off the window's bitmap, and a Glass paint read saw the old frame. Pump until the
    // content spans the client at the window's scale, for at most 3 s.
    bool settled = false;
    for (int i = 0; i < 25 && !settled; ++i) {
        RECT client{};
        GetClientRect(editor.window, &client);
        double scale = GetDpiForWindow(editor.window) / 96.0;
        editor.content.UpdateLayout();
        settled = editor.content.XamlRoot().RasterizationScale() == scale &&
                  std::abs(editor.content.ActualWidth() * scale - client.right) <= 1;
        if (!settled) PumpEditorFrames();
    }
    if (!settled) fprintf(stderr, "EDITOR-REPIN-UNSETTLED: the content did not take the pinned size in 3 s\n"), fflush(stderr);
    for (int i = 0; i < 4; ++i) PumpEditorFrames();
}

COLORREF EditorPixel(SettingsEditor& editor, FrameworkElement const& element, float x, float y) {
    HoldEditorPin(editor);
    auto point = element.TransformToVisual(editor.content).TransformPoint({x, y});
    double scale = editor.content.XamlRoot().RasterizationScale();
    RECT rect{}; assert(GetClientRect(editor.window, &rect));
    HDC dc = GetDC(editor.window), memory = CreateCompatibleDC(dc);
    HBITMAP bitmap = CreateCompatibleBitmap(dc, rect.right, rect.bottom);
    auto previous = SelectObject(memory, bitmap);
    assert(PrintWindow(editor.window, memory, 3));
    auto pixel = GetPixel(memory, static_cast<int>(std::lround(point.X * scale)),
                         static_cast<int>(std::lround(point.Y * scale)));
    SelectObject(memory, previous); DeleteObject(bitmap); DeleteDC(memory); ReleaseDC(editor.window, dc);
    assert(pixel != CLR_INVALID);
    return pixel;
}

// One PrintWindow read along the element's DIP row y from DIP x0 to x1, in whole window pixels. The element's
// own x axis is mapped through its transform to the window, so a scaled ancestor is accounted for.
struct EditorRow {
    double originPx{}, scale{};
    int firstPx{};
    std::vector<COLORREF> pixels;
    COLORREF at(int px) const { return pixels.at(px - firstPx); }
    double toPx(double dip) const { return originPx + dip * scale; }
    double toDip(double px) const { return (px - originPx) / scale; }
};
EditorRow ReadEditorRow(SettingsEditor& editor, FrameworkElement const& element, float y, float x0, float x1) {
    HoldEditorPin(editor);
    auto transform = element.TransformToVisual(editor.content);
    auto origin = transform.TransformPoint({0, y}), unit = transform.TransformPoint({100, y});
    double scale = editor.content.XamlRoot().RasterizationScale() * (unit.X - origin.X) / 100;
    RECT rect{}; assert(GetClientRect(editor.window, &rect));
    HDC dc = GetDC(editor.window), memory = CreateCompatibleDC(dc);
    HBITMAP bitmap = CreateCompatibleBitmap(dc, rect.right, rect.bottom);
    auto previous = SelectObject(memory, bitmap);
    assert(PrintWindow(editor.window, memory, 3));
    EditorRow row{origin.X * editor.content.XamlRoot().RasterizationScale(), scale};
    row.firstPx = static_cast<int>(std::lround(row.toPx(x0)));
    int lastPx = static_cast<int>(std::lround(row.toPx(x1)));
    int py = static_cast<int>(std::lround(origin.Y * editor.content.XamlRoot().RasterizationScale()));
    for (int px = row.firstPx; px <= lastPx; ++px) {
        auto pixel = GetPixel(memory, px, py);
        assert(pixel != CLR_INVALID);
        row.pixels.push_back(pixel);
    }
    SelectObject(memory, previous); DeleteObject(bitmap); DeleteDC(memory); ReleaseDC(editor.window, dc);
    return row;
}

// The sub-pixel window x of the magenta host's clip wall nearest expectedPx: the magenta crossing, moved by the
// partial coverage of the three pixels on each side, with the reference colours read six pixels into either
// side because the canvas behind the host is a gradient. rising: background on the left, magenta on the right.
double ClipWall(EditorRow const& row, double expectedPx, bool rising) {
    auto magenta = [&](int px) { auto p = row.at(px); return GetRValue(p) > 200 && GetBValue(p) > 200 && GetGValue(p) < 80; };
    int centre = static_cast<int>(std::lround(expectedPx)), best = INT_MIN;
    // The search and the coverage reads below stay on the row read: a scaled-down preview puts a wall near its start.
    int last = row.firstPx + static_cast<int>(row.pixels.size()) - 1;
    for (int k = std::max(centre - 32, row.firstPx + 8); k <= std::min(centre + 32, last - 7); ++k)
        if (magenta(k - 1) != rising && magenta(k) == rising && std::abs(k - centre) < std::abs(best - centre)) best = k;
    if (best == INT_MIN) {
        fprintf(stderr, "CLIP-WALL no %s crossing within 32 px of %.1f; row px %d..%d at %.4f px/DIP from %.2f, magenta runs:",
                rising ? "rising" : "falling", expectedPx, row.firstPx, last, row.scale, row.originPx);
        for (int k = row.firstPx; k <= last; ++k)
            if (magenta(k) && (k == row.firstPx || !magenta(k - 1))) {
                int end = k;
                while (end < last && magenta(end + 1)) ++end;
                fprintf(stderr, " %d-%d", k, end);
            }
        for (int k = std::max(centre - 32, row.firstPx); k <= std::min(centre + 32, last); k += 16)
            fprintf(stderr, "; %d #%02X%02X%02X", k, GetRValue(row.at(k)), GetGValue(row.at(k)), GetBValue(row.at(k)));
        fprintf(stderr, "\n");
    }
    assert(best != INT_MIN);
    COLORREF inside = row.at(rising ? best + 6 : best - 7), outside = row.at(rising ? best - 7 : best + 6);
    auto coverage = [&](int px) {
        auto p = row.at(px);
        double d[]{double(GetRValue(inside)) - GetRValue(outside), double(GetGValue(inside)) - GetGValue(outside),
                   double(GetBValue(inside)) - GetBValue(outside)};
        double c = ((GetRValue(p) - double(GetRValue(outside))) * d[0] + (GetGValue(p) - double(GetGValue(outside))) * d[1] +
                    (GetBValue(p) - double(GetBValue(outside))) * d[2]) / (d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
        return std::clamp(c, 0.0, 1.0);
    };
    double wall = best;
    for (int i = best - 3; i < best; ++i) wall -= rising ? coverage(i) : 1 - coverage(i);
    for (int i = best; i < best + 3; ++i) wall += rising ? 1 - coverage(i) : coverage(i);
    return wall;
}

template<class Predicate> void WaitFor(Predicate predicate, ULONGLONG milliseconds = 10000) {
    auto deadline = GetTickCount64() + milliseconds;
    while (!predicate()) {
        MSG message{};
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        bool late = GetTickCount64() >= deadline;
        if (late) fprintf(stderr, "WAIT-TIMEOUT: a %llu ms wait ran out\n", milliseconds), fflush(stderr);
        assert(!late);
        Sleep(5);
    }
}

// The editor opens on the mouse's monitor, and a window manager here tiles it when it is shown (1500x992 at (0, 114) and
// 1011x1327 at (2086, 107) were seen), after the pin at open while it was hidden. The checks that read pixels out of its
// preview were written for 1280x840 DIP on the primary, so ShownEditor pins it again once it is visible.
void PinEditorToPrimary() {
    HWND window = g_editorWindow.load();
    HMONITOR primary = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitor{sizeof(monitor)};
    if (!window || !GetMonitorInfoW(primary, &monitor)) return;
    if (MonitorFromWindow(window, MONITOR_DEFAULTTONULL) != primary)
        SetWindowPos(window, nullptr, monitor.rcWork.left, monitor.rcWork.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    WaitFor([&] { return MonitorFromWindow(window, MONITOR_DEFAULTTONULL) == primary; });
    RECT rect{}, target{};
    int placements = 0;
    do {
        UINT dpi = GetDpiForWindow(window);
        target = {monitor.rcWork.left, monitor.rcWork.top, monitor.rcWork.left + MulDiv(1280, dpi, 96),
                  monitor.rcWork.top + MulDiv(840, dpi, 96)};
        SetWindowPos(window, nullptr, target.left, target.top, target.right - target.left, target.bottom - target.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        ++placements;
        auto settled = GetTickCount64() + 500; // WM_DPICHANGED and a window manager may still move it.
        WaitFor([&] { return GetTickCount64() >= settled; });
        GetWindowRect(window, &rect);
    } while (!EqualRect(&rect, &target) && placements < 5);
    fprintf(stderr, "EDITOR-PLACED %ldx%ld at (%ld, %ld) at %u DPI after %d placement(s)%s\n", rect.right - rect.left,
            rect.bottom - rect.top, rect.left, rect.top, GetDpiForWindow(window), placements,
            EqualRect(&rect, &target) ? "" : "; EDITOR-PIN-LOST: the window did not hold 1280x840 DIP");
    fflush(stderr);
}

template<class Callback> void OnEditorAsync(Callback callback) {
    EditorTask task{callback, {}};
    assert(PostMessageW(g_editorWindow, kTestEditorMessage, 0, reinterpret_cast<LPARAM>(&task)));
    // A whole case: settings-save alone waits out LiquidHandoff's 5 s deadline. Its steps keep their 10 s bound.
    WaitFor([&] { return task.completed.load(); }, 60000);
    if (task.error) std::rethrow_exception(task.error);
}

std::vector<TrayCatalogItem> testTrayCatalog;
std::optional<TaskbarPreviewMetrics> testPreviewMetrics;
LRESULT CALLBACK TestDispatcher(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    if(window!=g_dispatch && message==WM_TIMER && wp==kLiquidTimerId) return 0;
    if (message == kQueryTrayCatalogMessage && (!livePreviewData || previewTrayDemo)) {
        { std::lock_guard lock(g_trayCatalogMutex); g_trayCatalog = testTrayCatalog; g_trayCatalogAvailable = true; }
        PostMessageW(reinterpret_cast<HWND>(wp), kTrayCatalogMessage, 0, 0);
        return 0;
    }
    if (message == kQueryPreviewMetricsMessage && !livePreviewData) {
        { std::lock_guard lock(g_previewMetricsMutex); g_previewMetrics = testPreviewMetrics.value_or(TaskbarPreviewMetrics{}); }
        PostMessageW(reinterpret_cast<HWND>(wp), kPreviewMetricsMessage, 0, 0);
        return 0;
    }
    return DispatchProc(window, message, wp, lp);
}
void CreateTestDispatcher() {
    WNDCLASSW cls{};
    cls.hInstance = GetModuleHandleW(nullptr);
    cls.lpfnWndProc = TestDispatcher;
    cls.lpszClassName = L"WindhawkSettingsCheckDispatcher";
    assert(RegisterClassW(&cls));
    g_dispatch = CreateWindowExW(0, cls.lpszClassName, L"", 0, 0, 0, 0, 0,
                                  HWND_MESSAGE, nullptr, cls.hInstance, nullptr);
    assert(g_dispatch);
    g_ownerThread = GetCurrentThreadId();
}

// Production plates live on the taskbar dispatcher thread. The settings test
// host has a separate UI thread, so give its plates the same dispatcher setup.
struct TestPlateDispatcher {
    HWND previous{g_dispatch}, window{};
    DWORD previousThread{g_ownerThread.load()};
    bool previousLiquidTimer{g_liquidTimerArmed};
    TestPlateDispatcher() {
        if(previousThread==GetCurrentThreadId()) return;
        window=CreateWindowExW(0,L"WindhawkSettingsCheckDispatcher",L"",0,0,0,0,0,
            HWND_MESSAGE,nullptr,GetModuleHandleW(nullptr),nullptr);
        assert(window);
        g_dispatch=window;g_ownerThread=GetCurrentThreadId();
        g_liquidTimerArmed=false;
    }
    ~TestPlateDispatcher() {
        if(!window) return;
        RefreshQueuedPlateFills();
        DestroyWindow(window);
        g_dispatch=previous;g_ownerThread=previousThread;
        g_liquidTimerArmed=previousLiquidTimer;
    }
};

int calls = 0;
void* expectedTarget{};
void* expectedOptions{};
HRESULT WINAPI ShowOriginal(void*, void* target) {
    assert(target == expectedTarget); ++calls; return S_FALSE;
}
HRESULT WINAPI ShowOptionsOriginal(void*, void* target, void* options) {
    assert(target == expectedTarget && options == expectedOptions);
    ++calls; return S_FALSE;
}

void CheckLiquidClose(SettingsEditor& ui) {
    ui.navigation.SelectedIndex(0);
    showPreview = true;
    ShowWindow(ui.child, SW_SHOW);
    SetWindowPos(ui.window, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    Controls::Grid host;
    host.Width(640); host.Height(48);
    host.HorizontalAlignment(HorizontalAlignment::Left);
    host.VerticalAlignment(VerticalAlignment::Top);
    host.Background(Media::SolidColorBrush(winrt::Windows::UI::Color{255,32,32,32}));
    ui.preview.canvas.Children().Append(host);
    ui.content.UpdateLayout();
    auto visual = Hosting::ElementCompositionPreview::GetElementVisual(host);
    auto compositor = visual.Compositor();
    auto clip = compositor.CreateRoundedRectangleGeometry();
    clip.Offset({40,4}); clip.Size({560,40}); clip.CornerRadius({12,12});
    visual.Clip(compositor.CreateGeometricClip(clip));
    PumpEditorFrames();
    auto outside = EditorPixel(ui, host, 10, 24);
    bool nativeCovered = false, previewClipped = false;
    for (bool native : {false, true}) {
        auto renderer = std::make_shared<LiquidRenderer>();
        assert(renderer->Initialize(compositor, nullptr, 640, 48));
        renderer->SetPreviewBackdrop(std::vector<uint32_t>(640*48, 0xFF0040C0));
        // Use an owned HWND to exercise runtime presentation, without
        // capturing any windows. The compositor still has the old
        // width while the sampled shader bounds already hold the target.
        renderer->taskbar = native ? ui.window : nullptr;
        if (native) renderer->FollowDock(clip);
        LiquidRenderer::LensConstants lens{};
        lens.center[0] = 320; lens.center[1] = 24;
        lens.halfSize[0] = 120; lens.halfSize[1] = 20;
        lens.radius = 12; lens.optics.opacity = 1; lens.optics.edgeHighlight = 1;
        Hosting::ElementCompositionPreview::SetElementChildVisual(host, renderer->visualRoot);
        renderer->SetMotionActive(true);
        assert(renderer->UpdateGeometry({0,0,640,48}, 640, 48, lens));
        PumpEditorFrames();
        auto pixel = EditorPixel(ui, host, 100, 24);
        assert(GetBValue(EditorPixel(ui, host, 320, 24)) > 100 &&
               GetRValue(EditorPixel(ui, host, 320, 24)) < 80); // Fixture really rendered the blue surface.
        bool covered = GetBValue(pixel) > 100 && GetBValue(pixel) > GetRValue(pixel) + 50;
        if (native) nativeCovered = covered;
        else previewClipped = !covered;
        assert(EditorPixel(ui, host, 10, 24) == outside); // Native outer clip remains authoritative.
        if (native) {
            auto rim = EditorPixel(ui, host, 43, 24);
            auto body = EditorPixel(ui, host, 180, 24);
            printf("CLOSE-PROFILE rim=%06lx body=%06lx\n", rim, body);
            fflush(stdout);
            assert(GetRValue(rim) > GetRValue(body) + 10 &&
                   GetRValue(body) < 10 && GetBValue(body) > 100);
        } // The shaded edge follows the live bounds, not the early sampled target.
        Hosting::ElementCompositionPreview::SetElementChildVisual(host, nullptr);
        renderer->Stop();
    }
    uint32_t index{}; assert(ui.preview.canvas.Children().IndexOf(host, index));
    ui.preview.canvas.Children().RemoveAt(index);
    ShowWindow(ui.window, SW_HIDE);
    showPreview = false;
    printf("CLOSE-CLIP native-covered=%d preview-clipped=%d\n", nativeCovered, previewClipped);
    fflush(stdout);
    assert(nativeCovered && previewClipped);
}

// Shows the editor at the bottom of the z-order on the Taskbar page for a check that reads rendered pixels or
// needs the preview renderer, and puts its visibility back afterwards: later checks keep the preview renderer of
// a visible editor.
struct ShownEditor {
    SettingsEditor& ui;
    bool wasVisible, previousShow;
    RECT pinned{};
    ShownEditor(SettingsEditor& ui) : ui(ui), wasVisible(IsWindowVisible(ui.window)), previousShow(showPreview) {
        ui.navigation.SelectedIndex(0);
        showPreview = true;
        ShowWindow(ui.child, SW_SHOW);
        SetWindowPos(ui.window, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        PinEditorToPrimary();
        GetWindowRect(ui.window, &pinned);
        g_editorPin = pinned;
        ui.content.UpdateLayout();
        UpdateEditorSurface(ui);
        for (int i = 0; i < 4; ++i) PumpEditorFrames();
    }
    ~ShownEditor() {
        g_editorPin = {};
        RECT now{};
        GetWindowRect(ui.window, &now);
        if (!EqualRect(&now, &pinned)) {
            fprintf(stderr, "EDITOR-MOVED during a pixel check: %ldx%ld at (%ld, %ld), pinned %ldx%ld at (%ld, %ld)\n",
                    now.right - now.left, now.bottom - now.top, now.left, now.top, pinned.right - pinned.left,
                    pinned.bottom - pinned.top, pinned.left, pinned.top);
            fflush(stderr);
        }
        if (!wasVisible) ShowWindow(ui.window, SW_HIDE);
        showPreview = previousShow;
    }
};

// The outline traced statically at a hull, as OutlinePath traces it, plus an 8x4 DIP tag above the band at the host's
// middle (the preview rounds its corners): a read that shows the tag is of a frame with this geometry.
CompositionPath TaggedTrace(DockOutline const& outline, DockHull const& hull) {
    winrt::com_ptr<ID2D1PathGeometry> geometry;
    winrt::check_hresult(OutlineFactory()->CreatePathGeometry(geometry.put()));
    winrt::com_ptr<ID2D1GeometrySink> sink;
    winrt::check_hresult(geometry->Open(sink.put()));
    sink->SetFillMode(D2D1_FILL_MODE_WINDING);
    TraceOutline(sink.get(), outline, hull);
    D2D1_POINT_2F tag[]{{324, 0}, {324, 4}, {316, 4}};
    sink->BeginFigure({316, 0}, D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLines(tag, 3);
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    winrt::check_hresult(sink->Close());
    auto source = winrt::make_self<GeometrySource>();
    source->geometry = geometry;
    return CompositionPath(source.as<winrt::Windows::Graphics::IGeometrySource2D>());
}

// Separate's two bars come from one path geometry morphed off the hull. Drive a hand-set hull through three
// positions and read the rendered clip: both bars must sit where the outline puts them at that hull.
void CheckSeparateMorph(SettingsEditor& ui) {
    ShownEditor shown(ui);
    Controls::Grid host;
    host.Width(640); host.Height(80);
    host.HorizontalAlignment(HorizontalAlignment::Left);
    host.VerticalAlignment(VerticalAlignment::Top);
    host.Background(Media::SolidColorBrush(winrt::Windows::UI::Color{255, 255, 0, 255}));
    ui.preview.canvas.Children().Append(host);
    host.Measure({640, 80}); host.Arrange({0, 0, 640, 80});
    ui.content.UpdateLayout();
    assert(host.ActualWidth() == 640 && host.ActualHeight() == 80);
    auto magenta = [](COLORREF pixel) { return GetRValue(pixel) > 200 && GetBValue(pixel) > 200 && GetGValue(pixel) < 80; };
    PumpEditorFrames();
    assert(magenta(EditorPixel(ui, host, 320, 40))); // The fixture renders and PrintWindow sees it before any clip.
    auto previousStyle = g_dockStyle;
    g_dockStyle = {.layout = DockLayout::Separate};
    Measurements m{.host = 640, .trayWidth = 150, .center = 320, .dockHeight = 80, .insets = {4, 4}};
    m.outline = MakeOutline(g_dockStyle, m.trayWidth, 12, 72, false, kContent);
    Root root;
    root.frame = host;
    assert(PrepareDock(root.dock, host, {4, 4}, m.outline));
    auto& dock = root.dock;
    auto path = dock.outline.try_as<CompositionPathGeometry>();
    assert(path && dock.clip.Geometry() == dock.outline && dock.outline != dock.geometry);
    assert(!path.TryGetAnimationController(L"Path")); // Keyed by the follower, not by PrepareDock.
    auto metrics = dock.visual.Compositor().CreatePropertySet();
    assert(FollowOutline(dock, metrics, m));
    assert(path.TryGetAnimationController(L"Path"));
    float from{}, span{};
    assert(metrics.TryGetScalar(L"morphFrom", from) == CompositionGetValueStatus::Succeeded && from == -640);
    assert(metrics.TryGetScalar(L"morphSpan", span) == CompositionGetValueStatus::Succeeded && span == 3 * 640);
    assert(!FollowOutline(dock, metrics, m)); // An unchanged key leaves the running morph alone.
    // The primary ultrawide's 5120 DIP host, whose 15360 DIP span magnifies any coarseness in the compositor's
    // Progress, then the fixture's own: every clip wall must land within half a pixel of the hull.
    for (float hostKey : {5120.f, 640.f}) {
        m.host = hostKey;
        assert(FollowOutline(dock, metrics, m)); // A changed host re-keys the morph.
        assert(metrics.TryGetScalar(L"morphSpan", span) == CompositionGetValueStatus::Succeeded && span == 3 * hostKey);
        for (float left : {40.f, 80.f, 120.f}) {
            dock.geometry.Offset({left, 4});
            dock.geometry.Size({2 * (320 - left), 72});
            PumpEditorFrames();
            // Button bar [L, R - (T + 16 + 12)], tray bar [R - (T + 16), R] with R = 640 - L and T = 150.
            float right = 640 - left, split = right - 178, trayLeft = right - 166;
            auto row = ReadEditorRow(ui, host, 40, 0, 640);
            double worst = 0;
            for (auto [wall, rising] : {std::pair{left, true}, std::pair{split, false}, std::pair{trayLeft, true}, std::pair{right, false}})
                worst = std::max(worst, std::abs(ClipWall(row, row.toPx(wall), rising) - row.toPx(wall)));
            printf("SEPARATE-MORPH host=%g left=%g bars [%g, %g] [%g, %g] worst wall error %.3f px\n", hostKey, left, left, split,
                   trayLeft, right, worst);
            assert(worst <= 0.5);
        }
    }
    m.trayWidth = 170;
    m.outline = MakeOutline(g_dockStyle, m.trayWidth, 12, 72, false, kContent);
    assert(FollowOutline(dock, metrics, m)); // A wider tray re-keys the morph.
    PumpEditorFrames();
    assert(magenta(EditorPixel(ui, host, 462 - 120 - 20 - 3, 40)) && !magenta(EditorPixel(ui, host, 462 - 120 - 20 + 3, 40)));
    // Edge arc's flares belong to the bars' walls, so they ride the same morph at the primary's host. The fixture's
    // screen edge is the band's bottom, 4 DIP above the host's, so one DIP past it each outer flare's tip is a straight
    // wall a radius out from its bar. On the edge row the flare covers its foot; the disc it curves around stays clear.
    m.host = 5120;
    m.trayWidth = 150;
    for (auto layout : {DockLayout::Unified, DockLayout::Separate}) {
        m.outline = MakeOutline({.shape = DockShape::EdgeArc, .layout = layout}, m.trayWidth, 12, 72, false, kContent);
        assert(FollowOutline(dock, metrics, m)); // A new outline re-keys the morph.
        bool separate = layout == DockLayout::Separate;
        for (float left : {60.f, 80.f, 120.f}) { // Every flare tip stays over 40 px from the row's ends for ClipWall.
            dock.geometry.Offset({left, 4});
            dock.geometry.Size({2 * (320 - left), 72});
            PumpEditorFrames();
            float right = 640 - left, split = right - 178, trayLeft = right - 166;
            std::vector<std::pair<float, bool>> walls{{left - 12, true}, {right + 12, false}};
            if (separate) walls.insert(walls.begin() + 1, {{split, false}, {trayLeft, true}});
            auto row = ReadEditorRow(ui, host, 77, 0, 640);
            double worst = 0;
            for (auto [wall, rising] : walls)
                worst = std::max(worst, std::abs(ClipWall(row, row.toPx(wall), rising) - row.toPx(wall)));
            printf("EDGE-ARC-MORPH %s host=5120 left=%g flare tips %g and %g worst wall error %.3f px\n",
                   separate ? "separate" : "unified", left, left - 12, right + 12, worst);
            assert(worst <= 0.5);
            assert(magenta(EditorPixel(ui, host, left - 6, 75.5f)) && magenta(EditorPixel(ui, host, right + 6, 75.5f)));
            assert(!magenta(EditorPixel(ui, host, left - 11, 65)) && !magenta(EditorPixel(ui, host, right + 11, 65)));
        }
    }
    puts("PASS: Edge arc's flares ride the outline's morph: at the primary's host each flare tip sits a radius out from "
         "its bar at every hull position, the foot is covered on the edge row and the disc is not");
    // A row read of a sloped antialiased wall moves with the layout's sub-pixel phase, so the sloped walls below are read
    // differentially: the same rows with the clip's geometry swapped for the outline traced statically at the hull the
    // fixture set, in the same fixture, compositor and phase. The morph's own clip comes back before its rows are read,
    // and the tag on the traced clip proves which frame a read saw.
    auto tracedRows = [&](std::vector<float> const& ys) {
        auto offset = dock.geometry.Offset();
        auto size = dock.geometry.Size();
        auto traced = dock.visual.Compositor().CreatePathGeometry(
            TaggedTrace(m.outline, {offset.x, offset.y, offset.x + size.x, offset.y + size.y}));
        auto tagged = [&] { return magenta(EditorPixel(ui, host, 320, 2)); };
        dock.clip.Geometry(traced);
        WaitFor(tagged, 2000);
        std::vector<EditorRow> rows;
        for (float y : ys) rows.push_back(ReadEditorRow(ui, host, y, 0, 640));
        dock.clip.Geometry(dock.outline);
        WaitFor([&] { return !tagged(); }, 2000);
        return rows;
    };
    auto rowCentre = [&](float y) { // The pixel row ReadEditorRow reads at y, by its centre in host DIP.
        auto transform = host.TransformToVisual(ui.content);
        double rasterScale = ui.content.XamlRoot().RasterizationScale();
        double originY = transform.TransformPoint({0, y}).Y * rasterScale;
        double hostPx = (transform.TransformPoint({100, y}).X - transform.TransformPoint({0, y}).X) / 100 * rasterScale;
        return float(y + (std::lround(originY) + .5 - originY) / hostPx); // The preview's Viewbox scales host DIP.
    };
    // Smaller tray's S hangs off the hull's right edge, so it rides the same morph. On the pixel row nearest half the
    // drop below the far side the S is the clip's falling wall, where the static trace's is and near the contract's x for
    // that row's centre.
    m.outline = MakeOutline({.trayReduction = 8}, m.trayWidth, 12, 72, false, kContent);
    assert(FollowOutline(dock, metrics, m));
    for (float left : {40.f, 80.f, 120.f}) {
        dock.geometry.Offset({left, 4});
        dock.geometry.Size({2 * (320 - left), 72});
        auto traced = tracedRows({8});
        auto row = ReadEditorRow(ui, host, 8, 0, 640);
        SmallerTrayProfile profile({left, 4, 640 - left, 76}, 12, 8, 150);
        double v = rowCentre(8) - 4, rho = profile.rho, d = profile.d;
        double x = v <= d / 2 ? profile.c1 + std::sqrt(rho * rho - (rho - v) * (rho - v))
                              : profile.c2 - std::sqrt(rho * rho - (v - (d - rho)) * (v - (d - rho)));
        double wall = ClipWall(row, row.toPx(x), false), drift = std::abs(wall - ClipWall(traced[0], row.toPx(x), false));
        double error = std::abs(wall - row.toPx(x));
        printf("SMALLER-TRAY-MORPH host=5120 left=%g S at row %.3f DIP: x %.3f DIP (centre %.3f), morphed - traced %.3f px, "
               "wall error %.3f px\n", left, v + 4, x, profile.s, drift, error);
        // The slanted walls' budgets, below. At the pinned layout the S read 0.02-0.07 px from its trace and 0.07-0.13 px
        // from the contract.
        assert(drift <= .3 && error <= 1);
    }
    puts("PASS: Smaller tray's S rides the outline's morph: at the primary's host its centre row's wall sits where the "
         "static trace puts it, near the contract's x, at every hull position");
    // Slanted ends ride it too, though the second keyframe's hull is inverted: 20 DIP in from the far side, on its
    // straight run at the Curve angle, each wall sits where the static trace puts it, near its own contract's x, moved in
    // clear of its end's plate, Separate's facing ones Bar distance apart where nearest; 2 DIP in the far fillet is on
    // its circle. The hulls leave the walls on the host. A fillet bent by the inverted keyframe hull misplaces the walls
    // by many pixels.
    for (float angle : {45.f, 70.f, 85.f}) {
        auto rounding = SlantRounding(DockShape::Island, 72, 0, 12, kDefaultConcave, angle);
        SlantedEnd buttonsEnd(DockShape::Island, 72, 0, 12, rounding, angle, EndPlate::buttons(72)),
            trayEnd(DockShape::Island, 72, 0, 12, rounding, angle, EndPlate::tray(72, 0));
        for (auto layout : {DockLayout::Unified, DockLayout::Separate}) {
            bool separate = layout == DockLayout::Separate;
            // At 45 degrees Separate's facing ends part the bars by some 190 DIP, past these hulls' room for buttons;
            // RunSlantedOutline checks it on a hull that fits. A 60 DIP tray leaves the buttons room at 70 and 85.
            if (separate && angle == 45) continue;
            float trayWidth = separate ? 60 : float(m.trayWidth);
            m.outline = MakeOutline({.layout = layout, .slant = 90 - angle}, trayWidth, 12, 72, false, kContent);
            assert(FollowOutline(dock, metrics, m));
            for (float left : {150.f, 170.f, 200.f}) {
                dock.geometry.Offset({left, 4});
                dock.geometry.Size({2 * (320 - left), 72});
                auto traced = tracedRows({24, 6});
                std::vector<EditorRow> rows{ReadEditorRow(ui, host, 24, 0, 640), ReadEditorRow(ui, host, 6, 0, 640)};
                float right = 640 - left, wallDepth = rowCentre(24) - 4, farDepth = rowCentre(6) - 4;
                float wall = buttonsEnd.wall(wallDepth), fillet = buttonsEnd.wall(farDepth);
                struct Wall { float x; bool rising; size_t row; };
                std::vector<Wall> walls{{left - wall, true, 0}, {right + trayEnd.wall(wallDepth), false, 0},
                                        {left - fillet, true, 1}, {right + trayEnd.wall(farDepth), false, 1}};
                if (separate) {
                    // The button bar's facing end leans right off its edge as the tray's end does, the tray bar's left as
                    // Start's does.
                    float gap = 12 + 16 + buttonsEnd.footprint() + trayEnd.footprint();
                    float buttonsEdge = right - trayWidth - gap, trayEdge = right - trayWidth - 16;
                    walls.push_back({buttonsEdge + buttonsEnd.wall(wallDepth), false, 0});
                    walls.push_back({trayEdge - trayEnd.wall(wallDepth), true, 0});
                }
                double drift = 0, sloped = 0;
                for (auto const& w : walls) {
                    auto const& row = rows[w.row];
                    double at = ClipWall(row, row.toPx(w.x), w.rising);
                    drift = std::max(drift, std::abs(at - ClipWall(traced[w.row], row.toPx(w.x), w.rising)));
                    sloped = std::max(sloped, std::abs(at - row.toPx(w.x)));
                }
                printf("SLANTED-MORPH %s%s host=5120 left=%g walls at %.3f DIP in %.3f out, far fillets at %.3f DIP in %.3f out: "
                       "morphed - traced worst %.3f px, sloped wall error %.3f px\n",
                       separate ? "separate" : "unified", angle == 45 ? "" : angle == 70 ? "-angle70" : "-angle85", left, wallDepth, wall,
                       farDepth, fillet, drift, sloped);
                // Over 16 sub-pixel phases: morphed - traced up to 0.19 px (Separate reads 0.000 at most walls), sloped
                // walls within 0.28 px of the contract.
                assert(drift <= .3 && sloped <= 1);
            }
        }
    }
    puts("PASS: slanted ends at 45, 70 and 85 degrees ride the outline's morph: at the primary's host each wall sits "
         "where the static trace puts it, near its contract's x, on its straight run and on its far fillet, Separate's "
         "facing ones Bar distance apart where nearest, at every hull position");
    auto highlights = GlassHighlights(dock.outline, DockTheme::Glass, false, DockBorder{{255, 0, 0, 0}, 2});
    for (uint32_t i = 0; i < highlights.Shapes().Size(); ++i)
        assert(highlights.Shapes().GetAt(i).as<CompositionSpriteShape>().Geometry() == dock.outline);
    GlassShadow shadow(ui.preview.canvas, host, dock.outline);
    assert(shadow.mask.Shapes().GetAt(0).as<CompositionSpriteShape>().Geometry() == dock.outline);
    auto original = dock.originalClip;
    RestoreDock(root);
    assert(!dock.outline && !dock.geometry && !path.TryGetAnimationController(L"Path"));
    assert(Hosting::ElementCompositionPreview::GetElementVisual(host).Clip() == original);
    g_dockStyle = previousStyle;
    uint32_t index{}; assert(ui.preview.canvas.Children().IndexOf(host, index));
    ui.preview.canvas.Children().RemoveAt(index);
    puts("PASS: Separate's outline is a path morphed off the hull: a paused controller bound to the hull, keyed once per "
         "outline, both bars rendered where the hull puts them, and restore releases the morph with the clip");
}

// The dock leads Windows' slide: the hull's left edge is Min(Start's animated edge, the eased layout-final edge
// `lead`). Bind the production hull expressions to a fixture hull with a stand-in Start visual and read the rendered
// clip walls through an open, Start's late slide, a close, a repeat, a retarget mid-ease and a lens probe.
void CheckDockLead(SettingsEditor& ui) {
    ShownEditor shown(ui);
    Controls::Grid host;
    host.Width(640); host.Height(80);
    host.HorizontalAlignment(HorizontalAlignment::Left);
    host.VerticalAlignment(VerticalAlignment::Top);
    host.Background(Media::SolidColorBrush(winrt::Windows::UI::Color{255, 255, 0, 255}));
    ui.preview.canvas.Children().Append(host);
    host.Measure({640, 80}); host.Arrange({0, 0, 640, 80});
    ui.content.UpdateLayout();
    assert(host.ActualWidth() == 640 && host.ActualHeight() == 80);
    auto previousStyle = g_dockStyle;
    g_dockStyle = {.layout = DockLayout::Separate};
    Measurements m{.host = 640, .trayWidth = 150, .center = 320, .dockHeight = 80, .insets = {4, 4}};
    m.outline = MakeOutline(g_dockStyle, m.trayWidth, 12, 72, false, kContent);
    Root root;
    root.frame = host;
    assert(PrepareDock(root.dock, host, {4, 4}, m.outline));
    auto compositor = root.dock.visual.Compositor();
    root.metrics = compositor.CreatePropertySet();
    // threshold -640 keeps the squeeze correction at 0, so S(x) = dockOrigin + shift + x - padding = x - 8.
    for (auto [name, value] : {std::pair{L"dockOrigin", 0.f}, std::pair{L"shift", 0.f}, std::pair{L"threshold", -640.f},
                               std::pair{L"padding", 8.f}, std::pair{L"margin", 4.f}, std::pair{L"height", 72.f},
                               std::pair{L"center", 320.f}})
        root.metrics.InsertScalar(name, value);
    assert(FollowOutline(root.dock, root.metrics, m));
    auto S = [](double x) { return x - 8; };
    const float L = 120; // Start's rest x; every wall below stays over 40 px from the row's ends for ClipWall.
    auto anchor = compositor.CreateSpriteVisual();
    anchor.Offset({L, 0, 0});
    auto magenta = [](COLORREF p) { return GetRValue(p) > 200 && GetBValue(p) > 200 && GetGValue(p) < 80; };
    struct Walls { double left, right, scale; };
    // Both hull walls in host DIP from one PrintWindow read: the outermost magenta crossings, refined by ClipWall.
    auto walls = [&] {
        auto row = ReadEditorRow(ui, host, 40, 0, 640);
        int first = row.firstPx, last = first + static_cast<int>(row.pixels.size()) - 1;
        int left = first; while (left <= last && !magenta(row.at(left))) ++left;
        int right = last; while (right > left && !magenta(row.at(right))) --right;
        assert(left + 16 < right);
        return Walls{row.toDip(ClipWall(row, left, true)), row.toDip(ClipWall(row, right + 1, false)), row.scale};
    };
    auto hullAt = [](Walls const& w, double left) { // Both walls of the hull {left, 640 - left} within half a pixel.
        return std::abs(w.left - left) * w.scale <= 0.5 && std::abs(w.right - (640 - left)) * w.scale <= 0.5;
    };
    auto pumpFor = [](ULONGLONG ms) { auto until = GetTickCount64() + ms; WaitFor([&] { return GetTickCount64() >= until; }); };

    LeadHull(root, S(L)); // The first call inserts the scalar the hull expressions read.
    assert(root.lead == S(L));
    BindHull(root.dock.geometry, root.metrics, anchor);
    PumpEditorFrames();
    auto w = walls();
    printf("DOCK-LEAD rest walls %.3f %.3f\n", w.left, w.right);
    assert(hullAt(w, S(L))); // Min(S, lead) is the identity at rest.

    // Open: the layout grows by one 56 DIP button. Start's slot moves 28 left, but its Visual has not slid yet.
    LeadHull(root, S(L - 28));
    auto controller = root.metrics.TryGetAnimationController(L"lead");
    assert(controller);
    controller.Pause();
    controller.Progress(1);
    PumpEditorFrames();
    w = walls();
    printf("DOCK-LEAD open walls %.3f %.3f with Start still at %g\n", w.left, w.right, L);
    assert(hullAt(w, S(L - 28))); // 0.4.1's Start-only hull leaves them at the rest walls.
    controller.Progress(0.5f);
    PumpEditorFrames();
    w = walls();
    printf("DOCK-LEAD half-eased left wall %.3f\n", w.left);
    assert(w.left > S(L - 28) + 2 && w.left < S(L) - 2); // The ease is on the hull.
    double held = w.left;
    LeadHull(root, S(L - 28)); // A repeat target is a no-op: a fresh ease would run on from here.
    PumpEditorFrames();
    w = walls();
    assert(std::abs(w.left - held) * w.scale <= 0.5);
    controller.Progress(1);
    anchor.Offset({L - 28, 0, 0}); // Windows' slide, about 300 ms late.
    PumpEditorFrames();
    w = walls();
    assert(hullAt(w, S(L - 28))); // The slide inside a grown bar moves nothing.

    // Close: the layout shrinks back while Start's Visual is still at L - 28.
    LeadHull(root, S(L));
    controller = root.metrics.TryGetAnimationController(L"lead");
    assert(controller);
    controller.Pause();
    controller.Progress(1);
    PumpEditorFrames();
    w = walls();
    printf("DOCK-LEAD close walls %.3f %.3f before Start moves\n", w.left, w.right);
    assert(hullAt(w, S(L - 28))); // Min keeps the Start-driven hull.
    anchor.Offset({L, 0, 0});
    PumpEditorFrames();
    w = walls();
    assert(hullAt(w, S(L))); // 0.4.1's close: the hull follows Start's slide.

    // Retarget: a second open 100 ms into the first, both eases running in real time. The left wall never
    // reverses, never steps faster than the ease's steepest slope allows, and settles on the second target.
    constexpr double kMaxSlope = 3.0 / 0.99; // dy/dx of the (0.33, 1), (0.68, 1) cubic at its start.
    struct Sample { ULONGLONG at; double left; };
    std::vector<Sample> samples{{GetTickCount64(), w.left}};
    double span = 28, worstRatio = 0;
    ULONGLONG retargetAt = 0;
    LeadHull(root, S(L - 28));
    auto started = GetTickCount64();
    while (GetTickCount64() - started < 750) {
        pumpFor(16);
        w = walls();
        samples.push_back({GetTickCount64(), w.left});
        if (!retargetAt && GetTickCount64() - started >= 100) {
            retargetAt = GetTickCount64();
            span = std::max(span, w.left - S(L - 56));
            LeadHull(root, S(L - 56));
        }
    }
    for (size_t i = 1; i < samples.size(); ++i) {
        double step = samples[i - 1].left - samples[i].left;
        // PrintWindow sees a frame up to two refreshes old, so each interval carries that slack.
        double allowed = kMaxSlope * span * double(samples[i].at - samples[i - 1].at + 34) / 333;
        worstRatio = std::max(worstRatio, step / allowed);
        assert(step >= -0.5 / w.scale && step <= allowed);
    }
    printf("DOCK-LEAD retarget %zu samples over %llu ms, retarget at %llu ms, settled %.3f, worst step/allowed %.2f\n",
           samples.size(), static_cast<unsigned long long>(samples.back().at - started),
           static_cast<unsigned long long>(retargetAt - started), samples.back().left, worstRatio);
    assert(samples.back().at - retargetAt >= 333 && hullAt(w, S(L - 56)));

    // Lens mid-lead: the hull is 56 DIP ahead of Start. The probe must encode the button's own position
    // (StripOrigin + button.Offset.X + centre), which 0.4.1's hull-minus-Start form would put 56 DIP left.
    auto renderer = std::make_shared<LiquidRenderer>();
    renderer->geometryVisual = compositor.CreateContainerVisual();
    renderer->geometryVisual.Size({8, 6});
    Controls::Grid probeHost;
    probeHost.Width(8); probeHost.Height(6);
    probeHost.HorizontalAlignment(HorizontalAlignment::Left);
    probeHost.VerticalAlignment(VerticalAlignment::Top);
    // The preview's Viewbox scales the canvas (0.98 at the pinned size, 0.54 to 0.71 in tiled windows), so a 2 DIP
    // sprite can be under 2 window pixels, with no pixel wholly inside it at some layout phases. Four times larger, a
    // sprite's centre pixel is inside at any phase.
    Media::ScaleTransform zoom;
    zoom.ScaleX(4); zoom.ScaleY(4);
    probeHost.RenderTransform(zoom);
    ui.preview.canvas.Children().Append(probeHost);
    Controls::Canvas::SetTop(probeHost, 84);
    ui.content.UpdateLayout();
    Hosting::ElementCompositionPreview::SetElementChildVisual(probeHost, renderer->geometryVisual);
    const float b = 300, nativeCenter = 22, clipCenter = 26;
    auto button = compositor.CreateSpriteVisual();
    button.Offset({b, 0, 0});
    renderer->TrackLens(0, button, anchor, host, root.metrics, nativeCenter, clipCenter);
    PumpEditorFrames();
    // Row 0 sits at y 2..4; each 2 DIP sprite from x encodes Floor((value + 32768) * 16) as RGB, read at its centre.
    auto decode = [&](float x) {
        auto p = EditorPixel(ui, probeHost, x + 1, 3);
        return double((uint32_t(GetRValue(p)) << 16) | (uint32_t(GetGValue(p)) << 8) | uint32_t(GetBValue(p))) / 16 - 32768;
    };
    double nativeX = decode(0), clipX = decode(2);
    printf("DOCK-LEAD lens native %.4f clip %.4f for button %g with the hull at %.3f and Start at %g\n",
           nativeX, clipX, b, w.left, L);
    assert(std::abs(nativeX - (b + nativeCenter)) <= 0.25 && std::abs(clipX - (b + clipCenter)) <= 0.25);
    Hosting::ElementCompositionPreview::SetElementChildVisual(probeHost, nullptr);
    uint32_t index{};
    assert(ui.preview.canvas.Children().IndexOf(probeHost, index));
    ui.preview.canvas.Children().RemoveAt(index);

    auto original = root.dock.originalClip;
    RestoreDock(root);
    assert(Hosting::ElementCompositionPreview::GetElementVisual(host).Clip() == original);
    g_dockStyle = previousStyle;
    assert(ui.preview.canvas.Children().IndexOf(host, index));
    ui.preview.canvas.Children().RemoveAt(index);
    puts("PASS: the dock leads Windows' slide: Min(Start, lead) is the identity at rest, an open grows both walls before "
         "Start moves and the slide then moves nothing, a close holds until Start moves, a repeat lead is a no-op, a "
         "retarget mid-ease is continuous within the ease's slope, and the lens reads the button's own position mid-lead");
}

// Smaller tray's band holds the tray content: trayVertical centres the tray in it, d/2 toward the screen edge, while
// the buttons keep the dock's band, and both tray plate call sites, PrepareStatePlates and the item-state refresh,
// fit a tray highlight's padding inside it.
void CheckTrayBand(SettingsEditor& ui) {
    for (bool atTop : {false, true}) {
        auto insets = PlanDockInsets(atTop, 48, 4);
        double drop = TrayDrop({.trayReduction = 8}, 48 - insets.top - insets.bottom);
        auto band = TrayInsets(insets, atTop, drop);
        double vertical = (insets.top - insets.bottom) / 2, trayVertical = (band.top - band.bottom) / 2;
        printf("TRAY-BAND %s vertical %.1f trayVertical %.1f\n", atTop ? "top" : "bottom", vertical, trayVertical);
        assert(drop == 8 && trayVertical == vertical + (atTop ? -4 : 4));
        assert(band.top + band.bottom == insets.top + insets.bottom + 8); // Only the far side grows.
        auto none = TrayInsets(insets, atTop, TrayDrop({}, 40));
        assert(none.top == insets.top && none.bottom == insets.bottom);
    }
    {
        // The follower inserts trayVertical and the tray's Translation reads it: a magenta tray strip's top edge moves
        // d/2 toward the screen edge, either way, while a cyan Start stand-in keeps the dock's vertical.
        ShownEditor shown(ui);
        Controls::Grid host, repeater;
        host.Width(640); host.Height(80);
        host.HorizontalAlignment(HorizontalAlignment::Left); host.VerticalAlignment(VerticalAlignment::Top);
        host.Background(Media::SolidColorBrush(winrt::Windows::UI::Color{255, 32, 32, 32}));
        Controls::Border start, strip;
        start.Name(L"LaunchListButton"); start.Width(640); start.Height(30); start.VerticalAlignment(VerticalAlignment::Top);
        start.Background(Media::SolidColorBrush(winrt::Windows::UI::Color{255, 0, 255, 255}));
        strip.Width(640); strip.Height(30); strip.Margin({0, 40, 0, 0}); strip.VerticalAlignment(VerticalAlignment::Top);
        strip.Background(Media::SolidColorBrush(winrt::Windows::UI::Color{255, 255, 0, 255}));
        repeater.Children().Append(start);
        host.Children().Append(repeater); host.Children().Append(strip);
        ui.preview.canvas.Children().Append(host);
        host.Measure({640, 80}); host.Arrange({0, 0, 640, 80}); ui.content.UpdateLayout(); PumpEditorFrames();
        auto magenta = [&](float y) { auto p = EditorPixel(ui, host, 320, y); return GetRValue(p) > 200 && GetBValue(p) > 200 && GetGValue(p) < 80; };
        auto cyan = [&](float y) { auto p = EditorPixel(ui, host, 320, y); return GetGValue(p) > 200 && GetBValue(p) > 200 && GetRValue(p) < 80; };
        assert(magenta(42) && !magenta(38) && cyan(28)); // The fixture renders before any translation.
        // The tray is the whole host wide, so its x clamps to 0 and only its y moves.
        Measurements m{.host = 640, .trayWidth = 640, .buttonsWidth = 640, .center = 320, .dockHeight = 80, .insets = {4, 4}};
        Root root;
        root.frame = host;
        for (bool atTop : {false, true}) {
            m.atTop = atTop;
            m.trayInsets = TrayInsets(m.insets, atTop, TrayDrop({.trayReduction = 8}, 72));
            m.outline = MakeOutline({.trayReduction = 8}, 100, 12, 72, atTop, kContent);
            if (!atTop) assert(PrepareDock(root.dock, host, m.insets, m.outline) && Prepare(repeater, root.buttonsTranslation) &&
                               Prepare(strip, root.trayTranslation));
            UpdateFollower(root, repeater, m);
            PumpEditorFrames();
            float vertical{}, trayVertical{};
            assert(root.metrics.TryGetScalar(L"vertical", vertical) == CompositionGetValueStatus::Succeeded &&
                   root.metrics.TryGetScalar(L"trayVertical", trayVertical) == CompositionGetValueStatus::Succeeded);
            float edge = 40 + trayVertical; // The strip's top edge.
            // Tray plates are placed through TransformToVisual, so record whether it sees the animated Translation.
            printf("TRAY-FOLLOWER %s vertical %.1f trayVertical %.1f: strip top %s %.0f, Start stand-in bottom %s 30; "
                   "TransformToVisual reads the strip's top at %.2f\n",
                   atTop ? "top" : "bottom", vertical, trayVertical, magenta(edge + 2) && !magenta(edge - 2) ? "at" : "NOT at",
                   edge, cyan(28) && !cyan(32) ? "at" : "NOT at", strip.TransformToVisual(host).TransformPoint({0, 0}).Y);
            assert(vertical == 0 && trayVertical == (atTop ? -4 : 4));
            assert(magenta(edge + 2) && !magenta(edge - 2) && cyan(28) && !cyan(32));
        }
        StopFollower(root);
        RestoreDock(root);
        uint32_t index{};
        assert(ui.preview.canvas.Children().IndexOf(host, index));
        ui.preview.canvas.Children().RemoveAt(index);
    }
    TestPlateDispatcher dispatcher;
    double previousPadding = g_trayHighlightPadding;
    g_trayHighlightPadding = 6;
    Controls::Grid background;
    background.Width(400); background.Height(48);
    background.HorizontalAlignment(HorizontalAlignment::Left); background.VerticalAlignment(VerticalAlignment::Top);
    Controls::StackPanel buttons, tray;
    tray.Orientation(Controls::Orientation::Horizontal);
    tray.HorizontalAlignment(HorizontalAlignment::Right); tray.VerticalAlignment(VerticalAlignment::Top);
    auto item = Markup::XamlReader::Load(LR"XAML(
<Button xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
 xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml" Width="40" Height="48" Padding="0" VerticalAlignment="Top">
 <Button.Template><ControlTemplate TargetType="Button">
  <Grid x:Name="Root">
   <VisualStateManager.VisualStateGroups>
    <VisualStateGroup x:Name="CommonStates">
     <VisualState x:Name="Normal"/>
     <VisualState x:Name="PointerOver"/>
    </VisualStateGroup>
   </VisualStateManager.VisualStateGroups>
   <Border x:Name="BackgroundElement" Height="48" CornerRadius="4" Background="Red"/>
  </Grid>
 </ControlTemplate></Button.Template>
</Button>)XAML").as<Controls::Button>();
    tray.Children().Append(item);
    background.Children().Append(buttons); background.Children().Append(tray);
    ui.preview.canvas.Children().Append(background);
    auto layout = [&] { background.Measure({400, 48}); background.Arrange({0, 0, 400, 48}); ui.content.UpdateLayout(); };
    layout(); item.ApplyTemplate(); layout(); PumpEditorFrames();
    assert(g_roots.empty());
    g_roots.resize(1);
    auto& root = g_roots[0];
    root.frame = background; root.dock.background = background;
    Measurements m{.dockHeight = 48, .insets = {4, 4}};
    m.trayInsets = TrayInsets(m.insets, false, TrayDrop({.trayReduction = 8}, 40));
    root.measurements = m; root.measurementsReady = true;
    PrepareStatePlates(root, buttons, tray, m);
    layout();
    auto state = std::find_if(root.statePlates.begin(), root.statePlates.end(), [](auto const& s) { return s.trayHighlight; });
    assert(state != root.statePlates.end() && state->templateValues && !state->blocked);
    auto plate = state->element;
    auto expected = GetButtonInsets(m.trayInsets, 48, g_trayHighlightPadding);
    auto placed = [&] { // The plate's state setters land on a later layout pass.
        layout(); PumpEditorFrames(); layout();
        double top = plate.TransformToVisual(background).TransformPoint({0, 0}).Y;
        return std::pair{top, top + plate.ActualHeight()};
    };
    auto [top, bottom] = placed();
    printf("TRAY-BAND plate %.2f..%.2f, Smaller tray's band with padding %.2f..%.2f\n", top, bottom, expected.top, 48 - expected.bottom);
    // Layout rounding snaps the plate to whole physical pixels: 18 DIP lands at 18.4 at 125 %.
    double pixel = 1 / ui.content.XamlRoot().RasterizationScale() + 1e-3;
    auto within = [&](double value, double target) { return std::abs(value - target) <= pixel; };
    assert(within(top, expected.top) && within(bottom, 48 - expected.bottom) && expected.top == 4 + 8 + 6);
    // The item-state refresh puts a tray plate back in the same band.
    PrepareStatePlate(root, *state, GetButtonInsets(m.insets, 48, g_trayHighlightPadding), false);
    assert(within(placed().first, 4 + 6));
    state->stateRefreshPending = true;
    RefreshItemStates();
    auto [refreshedTop, refreshedBottom] = placed();
    assert(!state->stateRefreshPending && within(refreshedTop, expected.top) && within(refreshedBottom, 48 - expected.bottom));
    for (auto& value : g_roots) RetireRoot(value);
    g_roots.clear();
    uint32_t index{};
    assert(ui.preview.canvas.Children().IndexOf(background, index));
    ui.preview.canvas.Children().RemoveAt(index);
    g_trayHighlightPadding = previousPadding;
    puts("PASS: Smaller tray's band moves the tray d/2 toward the screen edge on both taskbar edges: the follower's "
         "trayVertical drives the tray's Translation while the buttons keep the dock's vertical, and both tray plate "
         "call sites fit a tray highlight in the band");
}

// The frame's sizing without input. Each sizing hit maps to the cursor Windows shows for it and no other hit maps to
// one; SizedFromCursor moves only the hit's edges, by the cursor's travel, and stops them at the minimum size; each
// corner's sizing zone runs along both of its edges past the window region's rounding, so part of it is on screen.
void CheckResizeFrame(SettingsEditor& ui) {
    std::pair<int, LPCWSTR> const cursors[]{{HTLEFT, IDC_SIZEWE}, {HTRIGHT, IDC_SIZEWE}, {HTTOP, IDC_SIZENS},
        {HTBOTTOM, IDC_SIZENS}, {HTTOPLEFT, IDC_SIZENWSE}, {HTBOTTOMRIGHT, IDC_SIZENWSE}, {HTTOPRIGHT, IDC_SIZENESW},
        {HTBOTTOMLEFT, IDC_SIZENESW}};
    for (auto [hit, cursor] : cursors) assert(EditorSizingCursor(hit) == LoadCursorW(nullptr, cursor));
    for (int hit : {HTTRANSPARENT, HTNOWHERE, HTCLIENT, HTCAPTION, HTMINBUTTON, HTMAXBUTTON, HTBORDER, HTCLOSE})
        assert(!EditorSizingCursor(hit));

    RECT const start{100, 200, 1100, 900};
    SIZE const minimum{600, 400};
    struct Sizing { int hit; bool left, top, right, bottom; };
    Sizing const sizings[]{{HTLEFT, 1, 0, 0, 0}, {HTRIGHT, 0, 0, 1, 0}, {HTTOP, 0, 1, 0, 0}, {HTBOTTOM, 0, 0, 0, 1},
        {HTTOPLEFT, 1, 1, 0, 0}, {HTTOPRIGHT, 0, 1, 1, 0}, {HTBOTTOMLEFT, 1, 0, 0, 1}, {HTBOTTOMRIGHT, 0, 0, 1, 1}};
    for (auto const& s : sizings) {
        POINT press{s.left ? start.left + 2 : s.right ? start.right - 3 : 600,
                    s.top ? start.top + 2 : s.bottom ? start.bottom - 3 : 550};
        RECT inset{press.x - start.left, press.y - start.top, start.right - press.x, start.bottom - press.y};
        // The cursor travels on both axes whatever the hit; only the hit's edges follow it.
        LONG dx = s.left ? -40 : 40, dy = s.top ? -30 : 30;
        RECT pulled = SizedFromCursor(start, s.hit, {press.x + dx, press.y + dy}, inset, minimum);
        RECT const followed{start.left - (s.left ? 40 : 0), start.top - (s.top ? 30 : 0),
                            start.right + (s.right ? 40 : 0), start.bottom + (s.bottom ? 30 : 0)};
        assert(EqualRect(&pulled, &followed));
        // Pushed far past the opposite edges, each moving edge stops at the minimum size and the opposite edge stays.
        RECT pushed = SizedFromCursor(start, s.hit, {press.x - 5000 * (s.right - s.left),
                                                     press.y - 5000 * (s.bottom - s.top)}, inset, minimum);
        RECT const clamped{s.left ? start.right - 600 : start.left, s.top ? start.bottom - 400 : start.top,
                           s.right ? start.left + 600 : start.right, s.bottom ? start.top + 400 : start.bottom};
        assert(EqualRect(&pushed, &clamped));
    }
    puts("PASS: the frame's sizing hits map to Windows' sizing cursors (SIZEWE, SIZENS, SIZENWSE, SIZENESW) and no other "
         "hit maps to one; SizedFromCursor moves only each hit's edges by the cursor's travel and stops them at the "
         "minimum size with the opposite edge fixed, for all four edges and corners");

    ShownEditor shown(ui);
    RECT window{};
    assert(GetWindowRect(ui.window, &window));
    LONG width = window.right - window.left, height = window.bottom - window.top;
    HRGN region = CreateRectRgn(0, 0, 0, 0);
    assert(region && GetWindowRgn(ui.window, region) == COMPLEXREGION && ui.windowShapeRadius > 0);
    // Each corner pixel is cut away, so the 0.4.5 corner squares inside the sizing band were never on screen.
    assert(!PtInRegion(region, 0, 0) && !PtInRegion(region, width - 1, 0) && !PtInRegion(region, 0, height - 1) &&
           !PtInRegion(region, width - 1, height - 1));
    // The first column or row past the region's rounding (radius + 2 px) on both edges of each corner.
    LONG past = LONG(std::ceil(ui.windowShapeRadius)) + 2;
    struct Probe { LONG x, y; LRESULT hit; };
    Probe const probes[]{
        {width - 1 - past, height - 2, HTBOTTOMRIGHT}, {width - 2, height - 1 - past, HTBOTTOMRIGHT},
        {past, height - 2, HTBOTTOMLEFT}, {1, height - 1 - past, HTBOTTOMLEFT},
        {width - 1 - past, 1, HTTOPRIGHT}, {width - 2, past, HTTOPRIGHT},
        {past, 1, HTTOPLEFT}, {1, past, HTTOPLEFT},
        {width / 2, 1, HTTOP}, {width / 2, height - 2, HTBOTTOM}, {1, height / 2, HTLEFT}, {width - 2, height / 2, HTRIGHT},
        {width / 2, height / 2, HTCLIENT}};
    for (auto const& probe : probes) {
        LRESULT hit = SendMessageW(ui.window, WM_NCHITTEST, 0, MAKELPARAM(window.left + probe.x, window.top + probe.y));
        bool shown = PtInRegion(region, probe.x, probe.y);
        if (hit != probe.hit || !shown)
            printf("RESIZE-FRAME window point %ld,%ld hit=%lld (expected %lld) in region=%d\n", probe.x, probe.y,
                   static_cast<long long>(hit), static_cast<long long>(probe.hit), shown);
        assert(hit == probe.hit && shown);
    }
    DeleteObject(region);
    // Released, a frame resize's queued tail changes nothing: WM_SIZING proposes the window's own rect. A keyboard
    // resize has no grip and keeps Windows' proposal.
    assert(GetAsyncKeyState(GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON) >= 0);
    RECT const wider{window.left, window.top, window.right + 120, window.bottom};
    RECT proposed = wider;
    ui.sizeGrip = std::pair{HTRIGHT, RECT{width - 3, height / 2, 3, height - height / 2}};
    assert(SendMessageW(ui.window, WM_SIZING, WMSZ_RIGHT, reinterpret_cast<LPARAM>(&proposed)) == TRUE &&
           EqualRect(&proposed, &window));
    ui.sizeGrip.reset();
    proposed = wider;
    SendMessageW(ui.window, WM_SIZING, WMSZ_RIGHT, reinterpret_cast<LPARAM>(&proposed));
    assert(EqualRect(&proposed, &wider));
    printf("PASS: on the %ldx%ld editor window at %u DPI, with the region rounding %ld px off each corner, the points "
           "%ld px from a corner along either edge are that corner's sizing hit and inside the region, mid-edges and "
           "the body keep HTTOP, HTBOTTOM, HTLEFT, HTRIGHT and HTCLIENT, a released frame resize's WM_SIZING proposes "
           "the window's own rect and a keyboard resize keeps Windows' proposal\n", width, height, ui.dpi, past, past + 1);
}

// Separate in the preview: the shader draws two bars with the tray content in place, the Liquid shadow is masked
// by the same trace, and a Glass material is clipped to it with the border line as a composition stroke inside
// the clip. Unified puts the XAML Border back in charge.
// The live icon boxes off a stand-in tree on the preview canvas, laid out like the taskbar's: Start's icon is whatever
// is named Icon, and the tray's last item is the last icon view that shows content, past a zero-width one and Show
// desktop's strip at the tray's end, and past an earlier app icon whose content runs far beyond its slot (0.4.7: the
// right end ran to the screen edge). Insets come from Start's and the tray's own edges, depths from the band's far
// side after the vertical Translation that centres each band, counted once though our Translation on the repeater and
// the tray shows in TransformToVisual (0.4.7: the bell read 14 DIP deep).
void CheckDockIcons(SettingsEditor& ui) {
    Controls::Grid background;
    background.Width(400); background.Height(80);
    background.HorizontalAlignment(HorizontalAlignment::Left); background.VerticalAlignment(VerticalAlignment::Top);
    Controls::Grid repeater;
    repeater.HorizontalAlignment(HorizontalAlignment::Left); repeater.VerticalAlignment(VerticalAlignment::Top);
    Controls::Grid start;
    start.Width(44); start.Height(80); start.Margin({100, 0, 0, 0}); start.HorizontalAlignment(HorizontalAlignment::Left);
    Controls::Border icon;
    icon.Name(L"Icon"); icon.Width(24); icon.Height(24); icon.Margin({0, 10, 0, 0});
    icon.HorizontalAlignment(HorizontalAlignment::Center); icon.VerticalAlignment(VerticalAlignment::Top);
    start.Children().Append(icon);
    repeater.Children().Append(start);
    Controls::StackPanel tray;
    tray.Orientation(Controls::Orientation::Horizontal); tray.Margin({300, 0, 0, 0}); tray.HorizontalAlignment(HorizontalAlignment::Left);
    auto item = [](Controls::Panel const& parent, wchar_t const* name, double width, wchar_t const* glyph) {
        Controls::Grid grid;
        grid.Name(name); grid.Width(width);
        Controls::TextBlock text;
        text.Text(glyph); text.HorizontalAlignment(HorizontalAlignment::Center); text.VerticalAlignment(VerticalAlignment::Center);
        grid.Children().Append(text);
        parent.Children().Append(grid);
        return text;
    };
    // The first item shows nothing ContentBox knows, so it counts as its own box rather than not at all. Live, the first
    // item is the hidden icons' chevron, a SystemTray.ChevronIconView no test can make, so its match is read off the log.
    Controls::Grid first;
    first.Name(L"SystemTrayIcon"); first.Width(32);
    tray.Children().Append(first);
    item(tray, L"SystemTrayIcon", 32, L"A");
    // Drawn hundreds of times its size from its slot's left, as the live one reports itself (UIA 4772..7679 of a 48 px slot).
    auto wide = item(tray, L"NotifyItemIcon", 32, L"W");
    Media::ScaleTransform blowUp;
    blowUp.ScaleX(300); blowUp.ScaleY(300);
    wide.RenderTransform(blowUp);
    auto last = item(tray, L"NotifyItemIcon", 32, L"B");
    item(tray, L"SystemTrayIcon", 0, L"C");
    Controls::StackPanel showDesktop;
    showDesktop.Name(L"ShowDesktopStack");
    item(showDesktop, L"SystemTrayIcon", 12, L"D");
    tray.Children().Append(showDesktop);
    background.Children().Append(repeater); background.Children().Append(tray);
    ui.preview.canvas.Children().Append(background);
    ui.content.UpdateLayout();
    auto b = last.TransformToVisual(background).TransformBounds({0, 0, float(last.ActualWidth()), float(last.ActualHeight())});
    auto a = first.TransformToVisual(background).TransformBounds({0, 0, float(first.ActualWidth()), float(first.ActualHeight())});
    auto found = TrayItemBox(tray, background, true), foundFirst = TrayItemBox(tray, background, false);
    double trayRight = 300 + tray.ActualWidth();
    auto wideBox = wide.TransformToVisual(background).TransformBounds({0, 0, float(wide.ActualWidth()), float(wide.ActualHeight())});
    printf("DOCK-ICONS wide item at %.1f, %.1f wide (ActualWidth %.1f); tray right %.1f\n", wideBox.X, wideBox.Width,
           wide.ActualWidth(), trayRight);
    assert(wideBox.X + wideBox.Width > trayRight + 1000);
    auto matches = [](IconBox const& a, IconBox const& e) {
        return std::abs(a.inset - e.inset) < 1e-3 && std::abs(a.top - e.top) < 1e-3 && std::abs(a.bottom - e.bottom) < 1e-3;
    };
    for (bool atTop : {false, true}) {
        // Insets 4 and 8, the tray's band 8 shorter on the far side: the buttons' Translation is -2 DIP, the tray's 2 on a
        // bottom taskbar and -6 on a top one. Both are installed, with a sideways part as the live dock's.
        Measurements m{.trayWidth = tray.ActualWidth(), .dockHeight = 80, .insets = {4, 8},
                       .trayInsets = atTop ? DockInsets{4, 16} : DockInsets{12, 8}, .atTop = atTop};
        repeater.Translation({37, -2, 0});
        tray.Translation({-53, atTop ? -6.f : 2.f, 0});
        assert(std::abs(tray.TransformToVisual(background).TransformPoint({0, 0}).Y - tray.ActualOffset().y - (atTop ? -6 : 2)) < 1e-3);
        DockContent content = kContent;
        MeasureDockIcons(content, start, repeater, tray, background, m);
        IconBox startIcon = atTop ? IconBox{18, 40, 64} : IconBox{18, 4, 28};
        IconBox trayIcon = atTop ? IconBox{8 + trayRight - b.X - b.Width, 78 - b.Y - b.Height, 78 - b.Y}
                                 : IconBox{8 + trayRight - b.X - b.Width, b.Y - 2, b.Y + b.Height - 2};
        printf("DOCK-ICONS %s: Start icon %s (%.3f, %.3f, %.3f), last tray icon %s (%.3f, %.3f, %.3f); expected (%.3f, %.3f, %.3f) and "
               "(%.3f, %.3f, %.3f)\n", atTop ? "top" : "bottom", content.buttonIcon ? "found" : "missing",
               content.buttonIcon ? content.buttonIcon->inset : 0, content.buttonIcon ? content.buttonIcon->top : 0,
               content.buttonIcon ? content.buttonIcon->bottom : 0, content.trayIcon ? "found" : "missing",
               content.trayIcon ? content.trayIcon->inset : 0, content.trayIcon ? content.trayIcon->top : 0,
               content.trayIcon ? content.trayIcon->bottom : 0, startIcon.inset, startIcon.top, startIcon.bottom, trayIcon.inset,
               trayIcon.top, trayIcon.bottom);
        assert(content.buttonIcon && matches(*content.buttonIcon, startIcon));
        assert(content.trayIcon && matches(*content.trayIcon, trayIcon));
        // Separate's facing icons: with no task button the last button is Start, read from its right side (44 DIP, its
        // 24 DIP icon 10 DIP in), and the first tray item is the empty one's own box, from the tray's left edge at 300.
        IconBox firstIcon = atTop ? IconBox{8 + a.X - 300, 78 - a.Y - a.Height, 78 - a.Y} : IconBox{8 + a.X - 300, a.Y - 2, a.Y + a.Height - 2};
        assert(content.lastButtonIcon && matches(*content.lastButtonIcon, startIcon));
        assert(content.firstTrayIcon && matches(*content.firstTrayIcon, firstIcon));
    }
    assert(found && std::abs(found->X - b.X) < 1e-3 && std::abs(found->Width - b.Width) < 1e-3 && b.Width > 0);
    assert(foundFirst && std::abs(foundFirst->X - a.X) < 1e-3 && a.Width > 0);
    uint32_t index{};
    assert(ui.preview.canvas.Children().IndexOf(background, index));
    ui.preview.canvas.Children().RemoveAt(index);
    puts("PASS: the live icon boxes: Start's is the element named Icon, the tray's is the last item with content past a "
         "zero-width one and Show desktop's strip, as insets from Start's and the tray's edges and depths from the far side");
}

void CheckSeparatePreview(SettingsEditor& ui) {
    ShownEditor shown(ui);
    CheckDockIcons(ui);
    auto control = [&](PCWSTR key) -> EditorControl& { return ui.controls[&SettingDefinition(key) - kSettingFields]; };
    auto theme = control(L"theme").input.as<Controls::ComboBox>();
    auto layout = control(L"layout").input.as<Controls::ListBox>();
    auto borderOn = control(L"dockBorder").input.as<Controls::ToggleSwitch>();
    auto& shadowOpacity = control(L"liquid.shadowOpacity");
    int previousTheme = theme.SelectedIndex(), previousLayout = layout.SelectedIndex();
    bool previousBorder = borderOn.IsOn();
    double previousShadow = shadowOpacity.slider.Value();
    auto apply = [&](DockTheme t, int layoutIndex, bool border, double shadow) {
        ui.syncing = true;
        theme.SelectedIndex(int(t)); layout.SelectedIndex(layoutIndex);
        borderOn.IsOn(border); shadowOpacity.slider.Value(shadow);
        ui.syncing = false;
        UpdateEditorPreview(ui); ui.content.UpdateLayout(); PumpEditorFrames();
    };
    TaskbarPreviewMetrics metrics; metrics.dockHeight = 80; metrics.iconWidth = metrics.iconHeight = 40;
    ApplyPreviewMetrics(ui, metrics);
    apply(DockTheme::Liquid, 1, true, 50);
    assert(ui.preview.renderer && !ui.preview.unavailable);
    auto& material = ui.preview.material;
    float right = float(48 + material.Width()), split = right - 268, trayBar = right - 256; // 240 DIP of tray in 8 DIP padding.
    float mid = float(Controls::Canvas::GetTop(material)) + 40;
    assert(Controls::Canvas::GetLeft(ui.preview.icons[0]) == trayBar + 8); // The tray content keeps its place.
    auto covered = [&](float x) { return DockProbe(ui.preview.renderer, x, mid)[1]; };
    assert(covered(split - 3) == 1 && covered(split + 6) == 0 && covered(trayBar + 3) == 1 &&
           covered(right - 3) == 1 && covered(right + 3) == 0);
    assert(ui.preview.shadow && ui.preview.shadowGeometry.try_as<CompositionPathGeometry>() &&
           ui.preview.shadow->mask.Shapes().GetAt(0).as<CompositionSpriteShape>().Geometry() == ui.preview.shadowGeometry);
    apply(DockTheme::Glass, 1, true, 50);
    auto border = ReadBorder(EditorPageValues(ui, 0), L"dockBorder");
    auto visual = Hosting::ElementCompositionPreview::GetElementVisual(material);
    auto clip = visual.Clip().try_as<CompositionGeometricClip>();
    assert(clip && clip.Geometry().try_as<CompositionPathGeometry>() && material.BorderThickness().Left == 0);
    auto line = Hosting::ElementCompositionPreview::GetElementChildVisual(material).as<ShapeVisual>();
    auto stroke = line.Shapes().GetAt(0).as<CompositionSpriteShape>();
    assert(line.Shapes().Size() == 1 && stroke.Geometry() == clip.Geometry() && stroke.StrokeThickness() == 2 * border.width);
    apply(DockTheme::Glass, 0, true, 50);
    assert(!visual.Clip() && !Hosting::ElementCompositionPreview::GetElementChildVisual(material) &&
           material.BorderThickness().Left == border.width);
    apply(DockTheme::Liquid, 0, true, 50);
    assert(ui.preview.shadowGeometry.try_as<CompositionRoundedRectangleGeometry>()); // Unified keeps the rounded-rect mask.
    assert(covered(split + 6) == 1); // One dock: no gap.
    auto shape = control(L"shape").input.as<Controls::ListBox>();
    int previousShape = shape.SelectedIndex();
    ui.syncing = true; shape.SelectedIndex(2); ui.syncing = false;
    apply(DockTheme::Liquid, 0, true, 50);
    assert(ui.preview.shadowGeometry.try_as<CompositionPathGeometry>()); // Unified Edge arc's flares need the traced mask.
    // Without Liquid the material widens by the flare on each side and is clipped to the trace at the band, with the
    // border line as a composition stroke.
    apply(DockTheme::Glass, 0, true, 50);
    clip = visual.Clip().try_as<CompositionGeometricClip>();
    assert(clip && clip.Geometry().try_as<CompositionPathGeometry>() && material.BorderThickness().Left == 0 &&
           Hosting::ElementCompositionPreview::GetElementChildVisual(material) &&
           Controls::Canvas::GetLeft(material) == 48 - 12 && material.Width() == right - 48 + 24);
    // In the material's DIP the band's walls are at 12 and width - 12. A pixel shows the material when it follows the
    // material's paint, whatever the preview draws over or behind it. On the canvas bottom row each flare shows its
    // foot half the radius outside the wall; the disc it curves around and the corner above it stay clipped.
    apply(DockTheme::Glass, 0, false, 50);
    // EditorPixel reads the window pixel whose corner is nearest a point, so 0.6 DIP above the bottom read a pixel
    // straddling the canvas's bottom edge at some layout phases (2 of 10 at the pinned size). Each point reads the
    // window pixel that holds it, and lastRow is the centre of the last pixel row wholly on the material.
    double scale = ui.content.XamlRoot().RasterizationScale();
    auto holding = [&](float x, float y) {
        auto point = material.TransformToVisual(ui.content).TransformPoint({x, y});
        return EditorPixel(ui, ui.content, float(std::floor(point.X * scale) / scale), float(std::floor(point.Y * scale) / scale));
    };
    float width = float(material.Width()), bottom = float(material.Height());
    double bottomPx = material.TransformToVisual(ui.content).TransformPoint({0, bottom}).Y * scale;
    float lastRow = ui.content.TransformToVisual(material).TransformPoint({0, float((std::floor(bottomPx) - .5) / scale)}).Y;
    std::pair<float, float> points[]{{6, lastRow}, {width - 6, lastRow}, {1, bottom - 11}, {width - 1, bottom - 11}, {6, 6}};
    auto paint = [&](winrt::Windows::UI::Color color) {
        material.Background(Media::SolidColorBrush(color));
        ui.content.UpdateLayout(); PumpEditorFrames();
        std::vector<COLORREF> pixels;
        for (auto [x, y] : points) pixels.push_back(holding(x, y));
        return pixels;
    };
    auto red = paint({255, 255, 0, 0}), blue = paint({255, 0, 0, 255});
    auto shows = [&](size_t i) { return GetRValue(red[i]) - GetRValue(blue[i]) > 100 && GetBValue(blue[i]) - GetBValue(red[i]) > 100; };
    printf("EDGE-ARC-PAINT bottom row %.3f DIP above the bottom; red - blue in R, blue - red in B: feet %d/%d %d/%d, discs %d/%d "
           "%d/%d, corner %d/%d\n", bottom - lastRow, GetRValue(red[0]) - GetRValue(blue[0]), GetBValue(blue[0]) - GetBValue(red[0]),
           GetRValue(red[1]) - GetRValue(blue[1]), GetBValue(blue[1]) - GetBValue(red[1]), GetRValue(red[2]) - GetRValue(blue[2]),
           GetBValue(blue[2]) - GetBValue(red[2]), GetRValue(red[3]) - GetRValue(blue[3]), GetBValue(blue[3]) - GetBValue(red[3]),
           GetRValue(red[4]) - GetRValue(blue[4]), GetBValue(blue[4]) - GetBValue(red[4]));
    assert(shows(0) && shows(1) && !shows(2) && !shows(3) && !shows(4));
    // Smaller tray. The canvas bottom is the screen edge, so the tray side's far side, the top, comes d down: the
    // material is d lower over the tray than over the buttons, and the tray content centres d/2 lower.
    auto smaller = control(L"smallerTray").input.as<Controls::ToggleSwitch>();
    auto& amount = control(L"trayReduction");
    bool previousSmaller = smaller.IsOn();
    double previousAmount = amount.slider.Value();
    struct TrayContent { double icon, clock, highlightTop, highlightHeight; };
    auto trayContent = [&] {
        return TrayContent{Controls::Canvas::GetTop(ui.preview.icons[0]) + ui.preview.icons[0].Height() / 2,
                           Controls::Canvas::GetTop(ui.preview.clock), Controls::Canvas::GetTop(ui.preview.trayHighlight),
                           ui.preview.trayHighlight.Height()};
    };
    ui.syncing = true; shape.SelectedIndex(0); smaller.IsOn(false); amount.slider.Value(8); ui.syncing = false;
    apply(DockTheme::Liquid, 0, false, 0);
    auto before = trayContent();
    ui.syncing = true; smaller.IsOn(true); ui.syncing = false;
    apply(DockTheme::Liquid, 0, false, 0);
    auto after = trayContent();
    constexpr float d = 8;
    float top = float(Controls::Canvas::GetTop(material)), buttonX = 48 + 60,
          trayX = float(Controls::Canvas::GetLeft(ui.preview.icons[1])) + 14;
    auto liquidCovered = [&](float x, float y) { return DockProbe(ui.preview.renderer, x, y)[1]; };
    printf("SMALLER-TRAY-PREVIEW icon centre %.2f -> %.2f, clock %.2f -> %.2f, highlight %.2f+%.2f -> %.2f+%.2f\n",
           before.icon, after.icon, before.clock, after.clock, before.highlightTop, before.highlightHeight,
           after.highlightTop, after.highlightHeight);
    assert(liquidCovered(buttonX, top + 2) == 1 && liquidCovered(trayX, top + 2) == 0 &&
           liquidCovered(trayX, top + d - 1) == 0 && liquidCovered(trayX, top + d + 2) == 1);
    assert(after.icon == before.icon + d / 2 && after.clock == before.clock + d / 2 &&
           after.highlightTop == before.highlightTop + d && after.highlightHeight == before.highlightHeight - d);
    apply(DockTheme::Glass, 0, false, 0);
    clip = visual.Clip().try_as<CompositionGeometricClip>();
    assert(clip && clip.Geometry().try_as<CompositionPathGeometry>()); // The S needs the traced clip.
    float local = trayX - 48;
    std::pair<float, float> smallerPoints[]{{60, 2}, {local, 2}, {local, d - 1}, {local, d + 2}};
    std::copy(std::begin(smallerPoints), std::end(smallerPoints), std::begin(points));
    red = paint({255, 255, 0, 0}); blue = paint({255, 0, 0, 255});
    assert(shows(0) && !shows(1) && !shows(2) && shows(3));
    ui.syncing = true; smaller.IsOn(previousSmaller); amount.slider.Value(previousAmount); ui.syncing = false;
    puts("PASS: the Smaller tray preview clips the material d lower over the tray than over the buttons in Liquid and "
         "Glass, and moves the tray icons and clock d/2 lower and its highlight into the shorter band");
    // Slanted ends reach the S's footprint past the dock band. At a 68 DIP band the canvas widens so the material still
    // starts on it, and the shader covers the S on its straight run half the band in, at the Curve angle's slider value
    // and moved in clear of the first button by the page's padding and radius; slant off keeps the 0.4.6 canvas.
    {
        auto slant = control(L"slantedEnds").input.as<Controls::ToggleSwitch>();
        auto& curve = control(L"slantAngle");
        bool previousSlant = slant.IsOn();
        double previousAngle = curve.slider.Value();
        TaskbarPreviewMetrics tall = metrics;
        tall.dockHeight = 68;
        ApplyPreviewMetrics(ui, tall);
        double step = std::max(44.0, tall.iconWidth + 16) + 8, buttonWidth = step - 8, dockWidth = 5 * step + 284;
        auto plate = EndPlate::of(68, 0, control(L"buttonVerticalPadding").slider.Value(),
                                  std::min(control(L"buttonCornerRadius").slider.Value(), buttonWidth / 2));
        for (float angle : {45.f, 70.f, 85.f}) {
            ui.syncing = true; shape.SelectedIndex(2); slant.IsOn(true); curve.slider.Value(angle); ui.syncing = false;
            apply(DockTheme::Liquid, 0, false, 0);
            // The page's own icons beside the ends: the Start icon on the first button and the clock, 126 DIP wide and
            // centred 34 DIP tall in the tray band, its right edge 20 DIP in.
            auto pageStyle = ReadDockStyle(EditorPageValues(ui, 0));
            double drop = TrayDrop(pageStyle, 68);
            IconBox startIcon{16 + (buttonWidth - tall.iconWidth) / 2, (68 - tall.iconHeight) / 2, (68 + tall.iconHeight) / 2},
                clock{20, (68 + drop - 34) / 2, (68 + drop + 34) / 2};
            auto radii = SlantRounding(DockShape::EdgeArc, 68, float(drop), 12, float(pageStyle.concave), angle);
            SlantedEnd e(DockShape::EdgeArc, 68, 0, 12, radii, angle, EndPlate::icon(startIcon), kIconSpace),
                mirrored(DockShape::EdgeArc, 68, float(drop), 12, radii, angle, EndPlate::icon(clock), kIconSpace);
            float margin = float(ui.preview.margin), materialLeft = float(Controls::Canvas::GetLeft(material));
            float wallY = float(Controls::Canvas::GetTop(material)) + 34, wallX = margin - e.wall(34);
            float rightX = float(margin + dockWidth + mirrored.wall(34));
            printf("SLANTED-PREVIEW band 68 Edge arc%s: canvas %d wide (0.4.6 %g), margin %g (footprint %.3f), material left %.3f "
                   "width %.3f, button plate depth %g to %g radius %g, offset %.3f, S at (%.3f, %.3f); right end mirrored %.3f DIP "
                   "from the clock, offset %.3f, S at %.3f\n",
                   angle == 45 ? "" : angle == 70 ? " at 70 degrees" : " at 85 degrees", ui.preview.width, std::ceil(5 * step + 380),
                   margin, e.footprint(), materialLeft, material.Width(), plate.top, plate.bottom, plate.radius, e.offset, wallX, wallY,
                   mirrored.clearance, mirrored.offset, rightX);
            assert(materialLeft >= 0 && ui.preview.width == int(std::ceil(5 * step + 284 + 2 * margin)));
            assert(DockProbe(ui.preview.renderer, wallX + 3, wallY)[1] == 1 && DockProbe(ui.preview.renderer, wallX - 3, wallY)[1] == 0);
            assert(DockProbe(ui.preview.renderer, rightX - 3, wallY)[1] == 1 && DockProbe(ui.preview.renderer, rightX + 3, wallY)[1] == 0);
        }
        ui.syncing = true; slant.IsOn(false); ui.syncing = false;
        apply(DockTheme::Liquid, 0, false, 0);
        assert(ui.preview.width == int(std::ceil(5 * step + 380)) && ui.preview.margin == 48 &&
               Controls::Canvas::GetLeft(material) == 48 - 12);
        ui.syncing = true; slant.IsOn(previousSlant); curve.slider.Value(previousAngle); ui.syncing = false;
        ApplyPreviewMetrics(ui, metrics);
    }
    puts("PASS: the slanted preview widens the canvas by the S's footprint so the material starts on it and the shader covers "
         "the S on its straight run at the Curve angle, 45, 70 and 85 degrees, the left end 24 DIP beside the page's Start icon "
         "and the right end 24 DIP beside the clock, and slant off keeps the 0.4.6 canvas of 5 steps + 380 DIP");
    ui.syncing = true; shape.SelectedIndex(previousShape); ui.syncing = false;
    apply(DockTheme(previousTheme), previousLayout, previousBorder, previousShadow);
    puts("PASS: the Separate preview draws two shader bars around the tray content, masks the Liquid shadow with the same "
         "trace, clips the Glass material to it with a composition border line, and Unified hands the Border back");
    puts("PASS: the Edge arc preview masks the Liquid shadow with the trace, and a Glass material widens by its flares, "
         "clipped to the trace with a composition border, its flares showing on the canvas bottom row");
}

void CheckScreenAnchoredBackdrop(SettingsEditor& ui) {
    showPreview = true;
    ShowWindow(ui.child, SW_SHOWNOACTIVATE);
    SetWindowPos(ui.window, HWND_BOTTOM, 0,0,0,0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    Controls::Grid host;
    host.Width(640); host.Height(48);
    host.HorizontalAlignment(HorizontalAlignment::Left);
    host.VerticalAlignment(VerticalAlignment::Top);
    ui.preview.canvas.Children().Append(host);
    ui.content.UpdateLayout();
    auto visual = Hosting::ElementCompositionPreview::GetElementVisual(host);
    auto compositor = visual.Compositor();
    auto clip = compositor.CreateRoundedRectangleGeometry();
    clip.Offset({40,4}); clip.Size({560,40}); clip.CornerRadius({12,12});
    visual.Clip(compositor.CreateGeometricClip(clip));
    auto renderer = std::make_shared<LiquidRenderer>();
    assert(renderer->Initialize(compositor,nullptr,640,48));
    std::vector<uint32_t> pattern(640*48);
    for (size_t i=0; i<pattern.size(); ++i)
        pattern[i] = i%640 < 256 ? 0xFFCC2020 : 0xFF2020CC;
    renderer->SetPreviewBackdrop(pattern);
    renderer->taskbar = ui.window;
    renderer->FollowDock(clip);
    Hosting::ElementCompositionPreview::SetElementChildVisual(host,renderer->visualRoot);
    LiquidRenderer::LensConstants lens{};
    lens.center[0]=320; lens.center[1]=24;
    lens.halfSize[0]=120; lens.halfSize[1]=20; lens.radius=12;
    lens.optics.refraction=lens.optics.chroma=lens.optics.edgeHighlight=0;
    lens.optics.fresnel=lens.optics.specular=0;
    renderer->SetMotionActive(true);
    assert(renderer->UpdateGeometry({0,0,640,48},640,48,lens));
    for(int i=0;i<4;++i) PumpEditorFrames();
    auto wide = EditorPixel(ui,host,230,24);
    auto position = compositor.CreateVector2KeyFrameAnimation();
    auto size = compositor.CreateVector2KeyFrameAnimation();
    auto linear = compositor.CreateLinearEasingFunction();
    position.InsertKeyFrame(0,{40,4}); position.InsertKeyFrame(1,{200,4},linear);
    size.InsertKeyFrame(0,{560,40}); size.InsertKeyFrame(1,{240,40},linear);
    position.Duration(std::chrono::milliseconds(600)); size.Duration(position.Duration());
    clip.StartAnimation(L"Offset",position); clip.StartAnimation(L"Size",size);
    bool intermediate{};
    for(int i=0;i<7;++i) {
        PumpEditorFrames();
        auto pixel = EditorPixel(ui,host,230,24);
        assert(GetRValue(pixel)>150 && GetBValue(pixel)<80);
        std::lock_guard lock(renderer->mutex);
        if (renderer->haveCompositorBounds && renderer->compositorBounds[2] > 250 &&
            renderer->compositorBounds[2] < 550) intermediate = true;
    }
    assert(intermediate);
    auto narrow = EditorPixel(ui,host,230,24);
    printf("BACKDROP-ANCHOR wide=%06X narrow=%06X\n", unsigned(wide), unsigned(narrow));
    assert(GetRValue(wide)>150 && GetBValue(wide)<80);
    assert(GetRValue(narrow)>150 && GetBValue(narrow)<80);
    for (size_t i=0; i<pattern.size(); ++i)
        pattern[i] = i%640 < 256 ? 0xFF20CC20 : 0xFF2020CC;
    { std::lock_guard lock(renderer->mutex); renderer->taskbar = nullptr; }
    renderer->SetPreviewBackdrop(pattern);
    { std::lock_guard lock(renderer->mutex); renderer->taskbar = ui.window; }
    position.InsertKeyFrame(0,{200,4}); position.InsertKeyFrame(1,{40,4},linear);
    size.InsertKeyFrame(0,{240,40}); size.InsertKeyFrame(1,{560,40},linear);
    clip.StartAnimation(L"Offset",position); clip.StartAnimation(L"Size",size);
    for(int i=0;i<7;++i) {
        PumpEditorFrames();
        auto pixel = EditorPixel(ui,host,230,24);
        assert(GetGValue(pixel)>150 && GetRValue(pixel)<80);
    }
    puts("PASS: expansion uses the updated backdrop throughout the native animation");
    Hosting::ElementCompositionPreview::SetElementChildVisual(host,nullptr);
    auto geometryCapture = renderer->geometrySource;
    assert(geometryCapture && geometryCapture->active);
    renderer->Stop();
    assert(!geometryCapture->active);
    WaitFor([&] {std::lock_guard lock(geometryCapture->lifecycleMutex);return !geometryCapture->pool && !geometryCapture->session;});
    uint32_t index{}; assert(ui.preview.canvas.Children().IndexOf(host,index));
    ui.preview.canvas.Children().RemoveAt(index);
    ShowWindow(ui.window,SW_HIDE); showPreview=false;
}

IconPixels CaptureOpticsEditor(SettingsEditor& ui,bool desktop);
void SaveOpticsPixels(IconPixels const& image,std::filesystem::path const& path);

void CheckTrayPixels(std::wstring const& customFile,std::filesystem::path const& catalog,
                    std::filesystem::path const& directory) {
    Controls::Grid host{nullptr};
    Controls::Image image{nullptr};
    IconPixels original;
    Root owner;
    std::unique_ptr<TestPlateDispatcher> dispatcher;
    auto pixel=[&](SettingsEditor& ui,float x,float y) {
        auto capture=CaptureOpticsEditor(ui,false);
        auto point=image.TransformToVisual(ui.content).TransformPoint({x,y});
        double scale=ui.content.XamlRoot().RasterizationScale();
        int px=int(std::lround(point.X*scale)),py=int(std::lround(point.Y*scale));
        assert(px>=0 && py>=0 && px<int(capture.width) && py<int(capture.height));
        auto color=capture.pixels[py*capture.width+px];
        return RGB((color>>16)&255,(color>>8)&255,color&255);
    };
    // Return to the native UI loop between mutations and observations. Pumping
    // messages inside one callback does not guarantee a XAML render commit.
    auto awaitPixel=[&](auto predicate,float x=8,float y=32) {
        COLORREF result{};
        WaitFor([&] {
            OnEditorAsync([&](SettingsEditor& ui) {
                RefreshTrayImageLoads();
                result=pixel(ui,x,y);
            });
            return predicate(result);
        });
        return result;
    };
    OnEditorAsync([&](SettingsEditor& ui) {
        dispatcher=std::make_unique<TestPlateDispatcher>();
        owner.taskbar=ui.window;
        ui.navigation.SelectedIndex(0);showPreview=true;
        ShowWindow(ui.child,SW_SHOW);
        SetWindowPos(ui.window,HWND_TOPMOST,0,0,0,0,
            SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);
        host=Controls::Grid();host.Name(L"NotifyItemIcon");host.Width(64);host.Height(64);
        host.Background(Media::SolidColorBrush({255,20,40,100}));
        image=Controls::Image();image.Width(64);image.Height(64);
        original={64,64,std::vector<uint32_t>(64*64)};
        for(unsigned y=0;y<64;++y) for(unsigned x=0;x<64;++x)
            original.pixels[y*64+x]=x<16?0xFFFF0000:x<32?0x80800000:0;
        image.Source(IconBitmap(original));host.Children().Append(image);
        ui.preview.canvas.Children().Append(host);ui.content.UpdateLayout();
        g_grayscaleTrayIcons=true;PrepareTrayIcon(host,image,ui.window);
        assert(g_trayIcons.size()==1 && g_trayIcons[0].opacityOwned);
    });
    auto opaque=awaitPixel([](COLORREF color) {
        return abs(GetRValue(color)-GetGValue(color))<=1 &&
            abs(GetRValue(color)-GetBValue(color))<=1 && GetRValue(color)>10;
    });
    OnEditorAsync([&](SettingsEditor& ui) {
        auto partial=pixel(ui,24,32),hole=pixel(ui,48,32);
        printf("TRAY-PIXELS opaque=%06lX partial=%06lX hole=%06lX\n",opaque,partial,hole);
        assert(abs(GetRValue(partial)-37)<=3 && abs(GetGValue(partial)-47)<=3 && abs(GetBValue(partial)-77)<=3);
        assert(hole==RGB(20,40,100));
        for(auto& color:original.pixels) color=(color&0xFF000000)|((color>>8)&0x0000FF00);
        image.Source(IconBitmap(original));
    });
    awaitPixel([&](COLORREF color) {return GetRValue(color)>GetRValue(opaque)+80;});
    OnEditorAsync([&](SettingsEditor& ui) {
        RestoreTrayIcons(owner);
        assert(g_trayIcons.empty() && Hosting::ElementCompositionPreview::GetElementVisual(image).Opacity()==1);
        g_grayscaleTrayIcons=false;g_customTrayIcons[L""]={customFile,customFile};
        PrepareTrayIcon(host,image,ui.window);
        assert(g_trayIcons.size()==1 && g_trayIcons[0].custom[0] && g_trayIcons[0].custom[1]);
    });
    awaitPixel([](COLORREF color) {return GetBValue(color)>240 && GetRValue(color)<10 && GetGValue(color)<10;});
    OnEditorAsync([&](SettingsEditor& ui) {
        RestoreTrayIcons(owner);g_grayscaleTrayIcons=true;PrepareTrayIcon(host,image,ui.window);
    });
    awaitPixel([](COLORREF color) {
        return abs(GetRValue(color)-GetGValue(color))<=1 && abs(GetRValue(color)-GetBValue(color))<=1;
    });
    OnEditorAsync([&](SettingsEditor&) {
        SetModConfiguration(ParseSettingsToml("schemaVersion=1\n[settings.tray]\nenabled=false\ngrayscale=true\n"));
        PrepareTrayIcons(owner,host);
        assert(!g_grayscaleTrayIcons && g_customTrayIcons.empty() && g_trayIcons.empty());
        assert(Hosting::ElementCompositionPreview::GetElementVisual(image).Opacity()==1);
        assert(!Hosting::ElementCompositionPreview::GetElementChildVisual(host));
    });
    awaitPixel([](COLORREF color) {return GetGValue(color)>240;});
    auto white=(directory/L"runtime-white.png").wstring();
    auto black=(directory/L"runtime-black.png").wstring();
    OnEditorAsync([&](SettingsEditor& ui) {
        assert(CopyFileW((catalog/L"native-github-1.png").c_str(),white.c_str(),FALSE));
        assert(CopyFileW((catalog/L"native-github-2.png").c_str(),black.c_str(),FALSE));
        g_grayscaleTrayIcons=false;g_fontColorMode=FontColorMode::Adaptive;
        g_customTrayIcons[L""]={white,black};PrepareTrayIcon(host,image,ui.window);
    });
    WaitFor([&] {
        bool ready{};
        OnEditorAsync([&](SettingsEditor&) {ready=!RefreshTrayImageLoads();});
        return ready;
    });
    ForegroundSampleResult sample;
    OnEditorAsync([&](SettingsEditor& ui) {
        assert(DeleteFileW(white.c_str()) && DeleteFileW(black.c_str()));
        assert(g_roots.empty());g_roots.emplace_back();
        auto& root=g_roots.back();root.taskbar=ui.window;root.frame=ui.content;
        sample.taskbar=ui.window;sample.rootIdentity=reinterpret_cast<uintptr_t>(winrt::get_abi(root.frame));
        sample.configurationGeneration=g_configurationGeneration;sample.capturedAt=1000;
        sample.items={{reinterpret_cast<uintptr_t>(winrt::get_abi(g_trayIcons[0].element.get())),0.0}};
        ApplyAdaptiveForeground(sample);assert(g_trayIcons[0].light);
    });
    awaitPixel([](COLORREF color) {return GetRValue(color)>240;},8,8);
    OnEditorAsync([&](SettingsEditor&) {
        sample.items[0].luminance=1;sample.capturedAt+=g_adaptiveWindowMs;
        ApplyAdaptiveForeground(sample);assert(!g_trayIcons[0].light);
    });
    awaitPixel([](COLORREF color) {return GetRValue(color)<10;},8,8);
    OnEditorAsync([&](SettingsEditor&) {
        sample.items[0].luminance=0;--sample.configurationGeneration;ApplyAdaptiveForeground(sample);
        assert(!g_trayIcons[0].light);
        RestoreTrayIcons(owner);g_roots.clear();g_fontColorMode=FontColorMode::Default;g_customTrayIcons.clear();
    });
    awaitPixel([](COLORREF color) {return GetGValue(color)>240;});
    OnEditorAsync([&](SettingsEditor& ui) {
        uint32_t index{};assert(ui.preview.canvas.Children().IndexOf(host,index));ui.preview.canvas.Children().RemoveAt(index);
        ClearLoadedTrayViews();image=nullptr;host=nullptr;dispatcher.reset();
        SetWindowPos(ui.window,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
        ShowWindow(ui.window,SW_HIDE);showPreview=false;SetModConfiguration(ReadLocalSettings(g_settingsPath));
    });
    puts("PASS: adaptive tray artwork follows sampled background, rejects stale samples, and switches without files/network");
}

int RunChecks(int argc, wchar_t** argv) {
    setvbuf(stdout,nullptr,_IONBF,0);
    assert(argc == 2);
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    auto directory = std::filesystem::path(argv[1]);
    auto file = (directory / L"nested/settings.toml").wstring();
    auto customFile = (directory / L"custom.png").wstring();
    std::string simpleCatalog = R"([
        {"name":"Discord","source":"native","slug":"discord","data":{"aliases":[]}},
        {"name":"GitHub","source":"native","slug":"github","data":{"aliases":["git-hosting"]}},
        {"name":"Graphite","source":"native","slug":"graphite_editor","data":{"aliases":[]}}
    ])";
    std::string nativeMetadata = R"({"github":{"colors":{"light":"github-light","dark":"github"}}})";
    std::string simpleSvg = "<svg viewBox=\"0 0 24 24\"><path d=\"M0 0h24v24H0z M6 6v12h12V6z\"/></svg>";
    auto simpleDirectory = directory / L"clicked/dashboard-icons/v1";
    {
        auto entries = ParseDashboardCatalog(simpleCatalog, nativeMetadata);
        assert(entries.size() == 3 && entries[2].slug == L"graphite_editor");
        assert(!entries[0].FollowsTextColor() && entries[1].FollowsTextColor());
        auto variants = entries[1];
        variants.urls[2].clear(); // The original can serve as the dark variant.
        assert(variants.FollowsTextColor());
        variants.urls[2] = variants.urls[1]; // Identical variants do not adapt.
        assert(!variants.FollowsTextColor());
        variants.source = L"simpleicons"; // One SVG yields distinct white/black artwork.
        assert(variants.FollowsTextColor());
        assert(CatalogSearchText(L"GitHub") == L"github");
        assert(!CatalogIconSlug(L"../../secret") && !CatalogIconSlug(L"icon?token=1"));
        bool rejected = false;
        try { SimpleSvgPath("<svg viewBox=\"0 0 24 24\"><path d=\"M0 0&amp;bad\"/></svg>"); }
        catch (...) { rejected = true; }
        assert(rejected);
        auto path = SimpleSvgPath(simpleSvg);
        auto light = RenderSimpleIcon(path, "FFFFFF");
        auto dark = RenderSimpleIcon(path, "000000");
        assert(light.width == 128 && light.height == 128);
        assert(light.pixels[16 * 128 + 16] == 0xFFFFFFFF);
        assert(dark.pixels[16 * 128 + 16] == 0xFF000000);
        assert(light.pixels[64 * 128 + 64] == 0 && dark.pixels[64 * 128 + 64] == 0);
        assert(AllowedCatalogUrl(entries[1].urls[1]) && entries[1].urls[1].ends_with(L"github-light.png"));
        assert(!AllowedCatalogUrl(L"https://raw.githubusercontent.com.evil.test/homarr-labs/dashboard-icons/main/png/github.png"));
        assert(!AllowedCatalogUrl(L"https://cdn.jsdelivr.net/gh/selfhst/icons/png/../secret.png"));
        assert(!AllowedCatalogUrl(L"https://cdn.jsdelivr.net/gh/selfhst/icons/png/%2e%2e.png"));
        assert(WriteLocalFile((simpleDirectory / L"catalog.json").wstring(), simpleCatalog, true, kCatalogLimit));
        assert(WriteLocalFile((simpleDirectory / L"metadata.json").wstring(), nativeMetadata, true, kCatalogLimit));
        for (auto const& entry : entries) for (int variant = 0; variant < 3; ++variant) {
            auto stream = EncodeIconPng(variant == 2 ? dark : light);
            STATSTG info{}; assert(SUCCEEDED(stream->Stat(&info, STATFLAG_NONAME)));
            std::string png(static_cast<size_t>(info.cbSize.QuadPart), '\0');
            ULONG read{}; assert(SUCCEEDED(stream->Read(png.data(), static_cast<ULONG>(png.size()), &read)));
            assert(DecodeCatalogPng(png).width == 128);
            assert(WriteLocalFile((simpleDirectory / (L"native-" + entry.slug + L"-" +
                std::to_wstring(variant) + L".png")).wstring(), png, true, kIconDownloadLimit));
        }
        CatalogIconJob catalog;
        catalog.directory = simpleDirectory.wstring();
        RunSimpleJob(catalog);
        assert(catalog.entries.size() == 3);
        CatalogIconJob icon;
        icon.directory = simpleDirectory.wstring(); icon.entry = entries[1];
        RunSimpleJob(icon);
        assert(LoadTrayImage(icon.files.light).pixels == light.pixels);
        assert(LoadTrayImage(icon.files.dark).pixels == dark.pixels && icon.files.Adaptive());
        CatalogIconJob cached;
        cached.directory = simpleDirectory.wstring(); cached.entry = entries[1];
        RunSimpleJob(cached);
        assert(cached.pixels.pixels == light.pixels); // PNG alone is enough offline.
        cached.entry = entries[0];
        RunSimpleJob(cached);
        assert(!cached.files.Adaptive() && cached.files.light.ends_with(L"native-discord-0.png"));
        puts("PASS: Dashboard catalog parsing, URL validation, SVG/PNG decoding, and offline light/dark pairs");
    }
    {
        assert(CanonicalTrayId(L"00123") == L"123");
        assert(CanonicalTrayId(L"18446744073709551616").empty());
        assert(CanonicalTrayId(L"../icon").empty());
        auto local = ParseSettingsToml("schemaVersion=1\n[settings.tray]\ngrayscale=true\n"
            "[settings.tray.icons]\n\"00123\"='C:\\Icons\\app.png'\n");
        auto overrides = ReadTrayOverrides(local);
        assert(overrides.at(L"123").light == L"C:\\Icons\\app.png" && !overrides.at(L"123").Adaptive());
        assert(ReadTrayOverrides(ParseSettingsToml(SettingsDocument(local))) == overrides);
        overrides[L"456"] = {L"C:\\Icons\\white_2.png", L"C:\\Icons\\black_2.png"};
        AddTrayOverrides(local, overrides);
        assert(ReadTrayOverrides(ParseSettingsToml(SettingsDocument(local))) == overrides);
        assert(WriteLocalFile((directory / L"icon-pairs.toml").wstring(), SettingsDocument(local), true));
        assert(LocalBool(local, L"tray.grayscale"));
        assert(GrayscalePixel(0x80800000) >> 24 == 128);
        auto gray = GrayscalePixel(0xFF12A8EF);
        assert((gray & 255) == ((gray >> 8) & 255) && (gray & 255) == ((gray >> 16) & 255));
        assert(LoadTrayImage(customFile).width == 32);
        bool rejected = false;
        try { LoadTrayImage(L"https://example.com/icon.png"); } catch (...) { rejected = true; }
        assert(rejected);
        std::array<BYTE, 96> constructor{};
        BYTE registers[]{0x4D,0x8B,0xF0,0x48,0x8B,0xF9};
        std::copy_n(registers, sizeof(registers), constructor.begin());
        BYTE store[]{0x49,0x8B,0x06,0x48,0x89,0x87,0xB0,0,0,0};
        std::copy_n(store, sizeof(store), constructor.begin() + 64);
        assert(TrayPreferenceOffset(constructor) == 0xB0);
        constructor[0] = 0;
        assert(TrayPreferenceOffset(constructor) == 0);
        std::array<uintptr_t, 32> object{};
        g_trayPreferenceVtable = reinterpret_cast<void*>(1234);
        g_trayPreferenceOffset = 0xB0;
        object[0] = 1234; object[0xB0 / sizeof(uintptr_t)] = 987654321;
        assert(TrayPreferenceIdFromAbi(object.data() + 2) == 987654321);
        object[0] = 0;
        assert(TrayPreferenceIdFromAbi(object.data() + 2) == 0);
        g_trayPreferenceVtable = nullptr; g_trayPreferenceOffset = 0;
        puts("PASS: tray assignment TOML round-trip, local image validation, alpha-preserving grayscale, and checked persistent ID lookup");
    }
    {
        auto settings = ParseSettingsToml(
            "schemaVersion = 1\n"
            "[settings] # general\n"
            "cornerRadius = 34 # inline comment\n"
            "theme = 'glass'\n"
            "text = \"a # b = c\"\n"
            "large = +1_000\n"
            "[settings.button]\n"
            "border = false\n");
        assert(LocalInt(settings, L"cornerRadius") == 34);
        assert(!LocalBool(settings, L"button.border"));
        assert(settings.GetNamedNumber(L"large") == 1000);
        assert(settings.GetNamedString(L"text") == L"a # b = c");
        assert(LocalString(settings, L"theme") == L"glass");
        assert(LocalInt(settings, L"buttonCornerRadius") == 12);
        auto invalid = ParseSettingsToml(
            "schemaVersion = 1\n[settings]\n"
            "cornerRadius = \"34\"\nbuttonVerticalPadding = 1.5\nedgeDistance = 1e100\n"
            "\"button.border\" = 0\ntheme = \"bad\"\n");
        assert(LocalInt(invalid, L"cornerRadius") == 12);
        assert(LocalInt(invalid, L"buttonVerticalPadding") == 4);
        assert(LocalInt(invalid, L"edgeDistance") == 4);
        assert(LocalBool(invalid, L"button.border"));
        assert(LocalString(invalid, L"theme") == L"native");
        SetModConfiguration(nullptr);
        assert(g_theme == DockTheme::Native && g_fontColorMode == FontColorMode::Default);
        assert(g_dockCornerRadius == 12 && g_dockEdgeDistance == 4);
        assert(g_keepButtonRim && !g_grayscaleTrayIcons);
        using BorderColor = winrt::Windows::UI::Color;
        assert(g_dockBorder.width == 0); // The taskbar gets no border line unless asked.
        auto body = ReadBorder(nullptr, L"ui.bodyBorder");
        assert((body.width == 1 && body.color == BorderColor{255, 128, 128, 128})); // Settings body: on, Grey, 1 DIP.
        auto custom = ReadBorder(ParseSettingsToml(
            "schemaVersion=1\n[settings]\ndockBorder=true\ndockBorderColor=\"custom\"\n"
            "dockBorderCustomColor=\"#FF8000\"\ndockBorderWidth=3\n"), L"dockBorder");
        assert((custom.width == 3 && custom.color == BorderColor{255, 255, 128, 0}));
        assert((ReadBorder(ParseSettingsToml("schemaVersion=1\n[settings]\ndockBorder=true\n"
            "dockBorderColor=\"white\"\n"), L"dockBorder").color == BorderColor{255, 255, 255, 255}));
        assert(ReadBorder(ParseSettingsToml("schemaVersion=1\n[settings.ui]\nbodyBorder=false\n"),
            L"ui.bodyBorder").width == 0);
        auto faded = ReadBorder(ParseSettingsToml("schemaVersion=1\n[settings]\ndockBorder=true\n"
            "dockBorderWidth=0.5\ndockBorderOpacity=50\n"), L"dockBorder");
        assert((faded.width == 0.5f && faded.color == BorderColor{128, 128, 128, 128})); // Half-DIP line, 50% opacity.
        puts("PASS: border line defaults and color decoding");
        { std::ofstream out(directory / L"defaults.toml"); out << SettingsDocument(nullptr); }
        assert(windhawkSettingReads == 0); // Conflicting Windhawk values are never read.
        for (auto bad : {
                "schemaVersion=2\n[settings]\n",
                "schemaVersion=1\n[settings]\ntheme=\"unterminated\n",
                "schemaVersion=1\n[settings]\ncornerRadius=1\ncornerRadius=2\n",
                "schemaVersion=1\n[settings]\ncornerRadius=1__2\n"}) {
            bool rejected = false;
            try { ParseSettingsToml(bad); } catch (...) { rejected = true; }
            assert(rejected);
        }
        assert(!ReadLocalSettings(file));
        auto seed = SettingsDocument(settings);
        assert(EnsureSettingsFile(file, seed));
        { std::ofstream out(directory / L"generated.toml"); out << seed; }
        assert(LocalInt(ReadLocalSettings(file), L"cornerRadius") == 34);
        assert(EnsureSettingsFile(file, "schemaVersion=1\n")); // Never overwrite.
        assert(LocalInt(ReadLocalSettings(file), L"cornerRadius") == 34);
        std::string corrupt = "schemaVersion = 1\n[settings]\ntheme = \"broken";
        { std::ofstream out(std::filesystem::path(file), std::ios::binary | std::ios::trunc); out << corrupt; }
        assert(!ReadLocalSettings(file));
        assert(EnsureSettingsFile(file, seed));
        std::ifstream in{std::filesystem::path(file)};
        std::string preserved((std::istreambuf_iterator<char>(in)), {});
        assert(preserved == corrupt);
        puts("PASS: TOML comments/scalars/tables, typed fallback, and file preservation");
    }
    {
        DockStyle buttonArea{.alignment = DockAlignment::Buttons};
        // FitDock over the outline at Bar distance and at 0 on a 48 DIP band, as Refresh reads them, then Plan.
        auto fitted = [](double host, double buttons, double tray, DockStyle const& style) {
            DockStyle touching = style;
            touching.barGap = 0;
            return FitDock(host, buttons, tray, style, MakeOutline(style, tray, 12, 48, false, kContent),
                           MakeOutline(touching, tray, 12, 48, false, kContent));
        };
        // A 2000-DIP host with 600 DIP of buttons; natively centred on 1000, the tray at 1700.
        auto plan = [&](double tray, Placement& result, DockStyle const& style = {}) {
            return Plan(2000, 600, tray, 1000, 1700, 0, result, fitted(2000, 600, tray, style));
        };
        Placement group{}, buttons{};
        assert(plan(300, group) && plan(300, buttons, buttonArea));
        assert(group.buttonsX == -156 && group.trayX == -544);   // 544..1456: buttons, 12 DIP, tray centred on 1000.
        assert(buttons.buttonsX == 0 && buttons.trayX == -388);  // Buttons 700..1300 centred; the tray 12 DIP after them.
        // Off-centre: centred buttons leave 2000 - 1300 - 12 = 688 DIP for the tray. Past that the buttons move left
        // (0.4.8; Unified has no gap to narrow), until they would leave the host's left, where the dock falls back.
        assert(plan(688, buttons, buttonArea) && buttons.buttonsX == 0);
        assert(plan(688.5, buttons, buttonArea) && buttons.buttonsX == -.5 && buttons.trayX == -388.5);
        assert(plan(1388, buttons, buttonArea) && buttons.buttonsX == -700 && !plan(1388.5, buttons, buttonArea));
        assert(plan(688.5, group)); // The whole group still fits the host.
        assert(plan(1388, group) && !plan(1388.5, group));
        // The tray's end goes square as soon as the hull and its curve would leave the host, before anything moves: Edge
        // arc's 12 DIP flare fits 1000 + 300 + 12 + 668 + 8 + 12 = 2000, and 680 DIP of tray squares it in place.
        DockStyle arc{.shape = DockShape::EdgeArc, .alignment = DockAlignment::Buttons};
        auto curved = fitted(2000, 600, 668, arc), squared = fitted(2000, 600, 680, arc);
        assert(std::isnan(curved.square) && curved.buttonsCenter == 1000 && curved.gap == 12);
        assert(squared.square == 2000 - (1000 + 300 + 12 + 680 + 8) + kSquareSlack && squared.buttonsCenter == 1000 &&
               squared.gap == 12);
        // Native Start sits at min((W - B) / 2, R - B) in a W-wide repeater, so past the strip's right limit R it moves
        // twice as far. The half correction restores the natively centred position, which is independent of where the
        // buttons are placed, so Start lands at centre - B / 2 under both alignments. Repeater 500..2500 on a 3000-DIP host,
        // R = 1600, tray 400 DIP at 2200: the group's buttons centre on (3000 - 400 - 12) / 2 = 1294, the button area's on 1500.
        for (auto [style, middle] : {std::pair{DockStyle{}, 1294.0}, std::pair{buttonArea, 1500.0}}) {
            double overhang = NAN;
            double threshold = BoundaryThreshold(overhang, 1600 - 1400, 1400, 2000, 2200 - 500);
            assert(threshold == 2000 - 1600);
            for (double strip : {1000.0, 1400.0}) {
                double start = std::min((2000 - strip) / 2, 1600 - strip);
                double shift = ButtonsCenter(3000, 400, style) - (500 + 2000 / 2);
                assert(500 + start + shift + std::max(0.0, (threshold - start) / 2) == middle - strip / 2);
            }
        }
        puts("PASS: Button area centres the buttons with the tray 12 DIP after them, moves them left only as far as a "
             "wider tray needs, squares the tray's end before anything moves, and keeps the native right-limit half "
             "correction");
        // Separate, square: Bar distance's 12 DIP plus both bars' 8 DIP padding puts the tray content 28 DIP after the
        // last button. Where it doesn't fit, the gap narrows first, down to the 16 DIP of padding at Bar distance 0,
        // by twice the right side's overflow under Whole group as the group recentres, and only then do the buttons move.
        DockStyle separate{.layout = DockLayout::Separate};
        DockStyle separateButtons{.alignment = DockAlignment::Buttons, .layout = DockLayout::Separate};
        assert(MakeOutline({}, 300, 12, 48, false, kContent).gap == 12 && MakeOutline(separate, 300, 12, 48, false, kContent).gap == 28);
        Placement bars{}, barsButtons{};
        assert(plan(300, bars, separate) && plan(300, barsButtons, separateButtons));
        assert(bars.buttonsX == -164 && bars.trayX == -536);           // 536..1464: buttons, 28 DIP, tray centred on 1000.
        assert(barsButtons.buttonsX == 0 && barsButtons.trayX == -372); // Buttons 700..1300 centred; the tray 28 DIP after them.
        auto fits = fitted(2000, 600, 672, separateButtons), narrowed = fitted(2000, 600, 678, separateButtons),
             shifted = fitted(2000, 600, 690, separateButtons), groupNarrowed = fitted(2000, 600, 1372.5, separate);
        assert(fits.gap == 28 && fits.barGap == 12 && fits.buttonsCenter == 1000);
        assert(narrowed.gap == 22 && narrowed.barGap == 6 && narrowed.buttonsCenter == 1000);
        assert(shifted.gap == 16 && shifted.barGap == 0 && shifted.buttonsCenter == 994);
        // Whole group: 600 + 28 + 1372.5 overflows 2000 by 0.5 in all, 0.25 on the right once centred, so the gap narrows
        // by twice that.
        assert(groupNarrowed.gap == 27.5 && groupNarrowed.barGap == 11.5 && groupNarrowed.buttonsCenter == 300);
        // The narrowed outline puts the tray where FitDock did.
        DockStyle placed = separateButtons;
        placed.barGap = narrowed.barGap;
        assert(MakeOutline(placed, 678, 12, 48, false, kContent).gap == narrowed.gap);
        puts("PASS: Separate puts the tray content 28 DIP after the last button under both alignments, and on a host too "
             "narrow narrows its gap down to the facing ends touching before the buttons move left");
        auto edge = ParseSettingsToml("schemaVersion=1\n[settings]\nshape=\"edge\"\nalignment=\"buttons\"\n");
        assert(ReadDockStyle(edge).shape == DockShape::Edge && ReadDockStyle(edge).alignment == DockAlignment::Buttons);
        assert(ReadDockStyle(edge).layout == DockLayout::Unified);
        assert(ReadDockStyle(ParseSettingsToml("schemaVersion=1\n[settings]\nlayout=\"separate\"\n")).layout == DockLayout::Separate);
        assert(ReadDockStyle(ParseSettingsToml("schemaVersion=1\n[settings]\nshape=\"arc\"\n")).shape == DockShape::Island);
        TaskbarConfiguration_GetFrameSize_Original = [](int) -> double { return 48; };
        SetModConfiguration(nullptr);
        double islandFrame = TaskbarConfiguration_GetFrameSize_Hook(1);
        auto island = PlanDockInsets(false, islandFrame, g_dockEdgeDistance);
        SetModConfiguration(edge);
        double edgeFrame = TaskbarConfiguration_GetFrameSize_Hook(1);
        // Edge keeps the saved edge distance (4) but sits on the screen edge: no outer inset, on either edge.
        assert(LocalInt(edge, L"edgeDistance") == 4 && g_dockEdgeDistance == 0);
        auto bottom = PlanDockInsets(false, edgeFrame, g_dockEdgeDistance), top = PlanDockInsets(true, edgeFrame, g_dockEdgeDistance);
        assert(bottom.bottom == 0 && bottom.top == kDockInnerMargin && top.top == 0 && top.bottom == kDockInnerMargin);
        // The frame is 4 DIP shorter and the visible band keeps the island's 48 - 8 DIP.
        assert(islandFrame == 48 && edgeFrame == 44 && island.top == 4 && island.bottom == 4);
        assert(edgeFrame - bottom.top - bottom.bottom == islandFrame - island.top - island.bottom);
        SetModConfiguration(nullptr);
        TaskbarConfiguration_GetFrameSize_Original = nullptr;
        puts("PASS: Edge drops the outer inset, shortens the frame by the 4-DIP edge distance, and keeps the visible band");
        RunOutlineParity();
    }
    auto messaging = LoadLibraryW(L"CoreMessaging.dll");
    assert(messaging);
    auto createQueue = reinterpret_cast<decltype(&CreateDispatcherQueueController)>(
        GetProcAddress(messaging, "CreateDispatcherQueueController"));
    assert(createQueue);
    winrt::Windows::System::DispatcherQueueController queue{nullptr};
    winrt::check_hresult(createQueue(
        {sizeof(DispatcherQueueOptions), DQTYPE_THREAD_CURRENT, DQTAT_COM_NONE},
        reinterpret_cast<ABI::Windows::System::IDispatcherQueueController**>(winrt::put_abi(queue))));
    puts("Initializing standalone XAML host");
    auto xaml = Hosting::WindowsXamlManager::InitializeForCurrentThread();
    {
        Controls::Grid target;
        target.Name(L"SeparateQuickSettingsXamlControlCenter");
        Controls::MenuFlyout menu;
        for (auto text : {L"Task Manager", L"Taskbar settings", L"System settings"}) {
            Controls::MenuFlyoutItem item; item.Text(text); menu.Items().Append(item);
        }
        g_ownerThread = GetCurrentThreadId();
        FlyoutShowAt_Original = ShowOriginal;
        FlyoutShowAtOptions_Original = ShowOptionsOriginal;
        expectedTarget = winrt::get_abi(target);
        expectedOptions = reinterpret_cast<void*>(1234);
        auto base = menu.as<Controls::Primitives::IFlyoutBase>();
        auto extended = menu.as<Controls::Primitives::IFlyoutBase5>();
        assert(FlyoutShowAt_Hook(winrt::get_abi(base), expectedTarget) == S_FALSE);
        assert(menu.Items().Size() == 5);
        assert(menu.Items().GetAt(4).as<Controls::MenuFlyoutItem>().Text() == L"Ultrawide taskbar settings...");
        auto retained = menu.Items().GetAt(4).as<Controls::MenuFlyoutItem>();
        testTrayCatalog = {{L"123", L"Sample app", LoadTrayImage(customFile), false},
                           {L"456", L"Overflow app", {}, true}};
        g_settingsPath = (directory / L"clicked/settings.toml").wstring();
        assert(EnsureSettingsFile(g_settingsPath, "schemaVersion = 1\n[settings]\ncornerRadius = 29\n"));
        SetModConfiguration(ReadLocalSettings(g_settingsPath));
        CreateTestDispatcher();
        Automation::Peers::MenuFlyoutItemAutomationPeer peer(retained);
        auto invoke = peer.GetPattern(Automation::Peers::PatternInterface::Invoke)
            .as<Automation::Provider::IInvokeProvider>();
        invoke.Invoke();
        WaitFor([] { return g_editorWindow.load() != nullptr; }); PinEditorToPrimary();
        HWND editor = g_editorWindow;
        testEditorOriginal = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(
            editor, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(TestEditorProc)));
        assert(testEditorOriginal);
        assert(LocalInt(ReadLocalSettings(g_settingsPath), L"cornerRadius") == 29);
        invoke.Invoke();
        assert(g_editorWindow == editor); // Single settings window.
        OnEditorAsync([&](SettingsEditor& ui) {
            showPreview=true;
            ShowWindow(ui.child,SW_SHOWNOACTIVATE);
            SetWindowPos(ui.window,HWND_BOTTOM,0,0,0,0,
                SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);
            ui.content.UpdateLayout();UpdateEditorSurface(ui);UpdateEditorPreview(ui);
            assert(ui.island && ui.content && ui.controls.size()==std::size(kSettingFields));
            std::set<uintptr_t> inputs;
            constexpr PCWSTR fieldPanels[]{L"AppearanceFields",L"LiquidFields",L"TrayFields",L"UiFields"};
            for(size_t i=0;i<std::size(kSettingFields);++i) {
                auto const& control=ui.controls[i];auto const& field=kSettingFields[i];
                assert(control.card && control.row && control.input);
                assert(inputs.insert(reinterpret_cast<uintptr_t>(winrt::get_abi(control.input))).second);
                assert(Automation::AutomationProperties::GetAutomationId(control.input)==field.key);
                assert(Automation::AutomationProperties::GetName(control.input)==field.label);
                auto panel=ui.content.FindName(fieldPanels[field.page]).as<Controls::StackPanel>();
                auto contains=[&](auto&& self,Controls::Panel const& parent)->bool {
                    uint32_t index{};
                    if(parent.Children().IndexOf(control.card,index)) return true;
                    for(auto const& child:parent.Children())
                        if(auto nested=child.try_as<Controls::Panel>();nested && self(self,nested)) return true;
                    return false;
                };
                // A collapsed group can have no realized visual parent yet.
                assert(contains(contains,panel));
                if(field.kind==SettingKind::Choice)
                    assert(control.input.as<Controls::ComboBox>().Items().Size()==field.choices.size());
                else if(field.kind==SettingKind::Segmented) {
                    auto items=control.input.as<Controls::ListBox>().Items();
                    assert(items.Size()==field.choices.size());
                    for(uint32_t j=0;j<items.Size();++j)
                        assert(winrt::unbox_value<winrt::hstring>(items.GetAt(j).as<Controls::ListBoxItem>().Content())==
                               field.choices[j].label);
                }
                else if(field.kind==SettingKind::Toggle) assert(control.input.try_as<Controls::ToggleSwitch>());
                else {
                    assert(control.input.try_as<Controls::TextBox>());
                    if(field.kind==SettingKind::Number) assert(control.slider);
                }
            }
            printf("PASS: all %zu declared settings have distinct, named controls on their declared pages\n",inputs.size());
            assert(ui.preview.canvas && ui.preview.icons.size() == 3 && ui.preview.appIcons.size() == 4);
            std::set<uint64_t> appPixels;
            for (auto const& image : ui.preview.appIcons) {
                auto bitmap = image.Source().as<Media::Imaging::WriteableBitmap>();
                assert(bitmap.PixelWidth() >= 32 && bitmap.PixelHeight() >= 32);
                byte* pixels{};
                winrt::check_hresult(bitmap.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>()->Buffer(&pixels));
                uint64_t hash = 14695981039346656037ull;
                for (uint32_t i = 0; i < bitmap.PixelBuffer().Capacity(); ++i)
                    hash = (hash ^ pixels[i]) * 1099511628211ull;
                appPixels.insert(hash);
            }
            assert(appPixels.size() == 4);
            assert(ui.controls[3].input.as<Controls::TextBox>().Text() == L"29");
            ui.controls[3].input.as<Controls::TextBox>().Text(L"6");
            assert(ui.preview.material.CornerRadius().TopLeft == 6);
            assert(g_dockCornerRadius == 29 && LocalInt(ReadLocalSettings(g_settingsPath), L"cornerRadius") == 29);
            ui.controls[3].input.as<Controls::TextBox>().Text(L"42");
            assert(ui.controls[3].slider.Value() == 42 && ui.dirty);
            auto panelBlend = [&] {
                assert(ui.backgroundRenderer);
                std::lock_guard lock(ui.backgroundRenderer->mutex);
                return ui.backgroundRenderer->lens.lensBlend;
            };
            assert(panelBlend() == 1.0f);
            ui.controls[&SettingDefinition(L"ui.panelOpacity") - kSettingFields].slider.Value(40);
            UpdateEditorSurface(ui);
            assert(panelBlend() == 1.0f); // A Settings UI draft leaves the window unchanged.
            InvokeSave(ui);
        });
        WaitFor([] { return g_dockCornerRadius == 42; });
        OnEditorAsync([&](SettingsEditor& ui) {
            std::lock_guard lock(ui.backgroundRenderer->mutex);
            assert(std::abs(ui.backgroundRenderer->lens.lensBlend - 0.4f) < 1e-6f); // Saved: panels at 40%.
        });
        assert(LocalInt(ReadLocalSettings(g_settingsPath), L"ui.panelOpacity") == 40);
        puts("PASS: Settings UI page applies on save, not while editing");
        auto pumpFor = [](int ms) { auto end = GetTickCount64() + ms; WaitFor([&] { return GetTickCount64() >= end; }); };
        pumpFor(300);
        OnEditorAsync([&](SettingsEditor& ui) {
            ui.hovered = nullptr;
            ui.content.UpdateLayout();
            UpdateEditorSurface(ui, false);
            float scale = GetDpiForWindow(ui.window) / 96.0f;
            auto shape = ui.controls[&SettingDefinition(L"shape") - kSettingFields].input.as<Controls::ListBox>();
            // The content layer draws plates within half a viewport of the view; the switch sits below that
            // on a short screen, so bring it into view first.
            auto inContent = shape.TransformToVisual(ui.scroll.Content().as<UIElement>()).TransformPoint({0, 0}).Y;
            ui.scroll.ChangeView(nullptr, std::max(0.0f, inContent - 200), nullptr, true);
            ui.content.UpdateLayout();
            PumpEditorFrames();
            UpdateEditorSurface(ui, false);
            auto segment = [&](uint32_t index) { return shape.Items().GetAt(index).as<FrameworkElement>(); };
            // One row: the second segment starts right of the first, on the same line.
            auto second = segment(1).TransformToVisual(segment(0)).TransformPoint({0, 0});
            assert(segment(0).ActualWidth() > 0 && second.X > segment(0).ActualWidth() - .5 && second.Y == 0);
            // Index into EditorLenses of the glass lens on this element with this corner radius and tint opacity.
            auto lensOn = [&](FrameworkElement const& element, float radius, int alpha) {
                auto at = element.TransformToVisual(ui.content).TransformBounds(
                    {0, 0, float(element.ActualWidth()), float(element.ActualHeight())});
                auto lenses = EditorLenses(ui);
                for (size_t i = 0; i < lenses.size(); ++i)
                    if (lenses[i].kind == 2 && std::abs(lenses[i].native[0] - (at.X + at.Width / 2) * scale) < 1 &&
                        std::abs(lenses[i].native[1] - (at.Y + at.Height / 2) * scale) < 1 &&
                        std::abs(lenses[i].native[2] - at.Width * scale / 2) < 1 &&
                        std::abs(lenses[i].radius - radius * scale) < .5f &&
                        std::abs(lenses[i].alpha - alpha / 255.0f) < .01f) return int(i);
                return -1;
            };
            int knob = lensOn(segment(0), 6, 110), track = lensOn(shape, 8, 40);
            if (!(knob >= 0 && track > knob && lensOn(segment(1), 6, 110) < 0)) {
                auto at = segment(0).TransformToVisual(ui.content).TransformBounds(
                    {0, 0, float(segment(0).ActualWidth()), float(segment(0).ActualHeight())});
                fprintf(stderr, "segment0 at %.1f,%.1f %.1fx%.1f scale %.2f knob=%d track=%d\n", at.X, at.Y, at.Width, at.Height, scale, knob, track);
                auto lenses = EditorLenses(ui);
                for (size_t i = 0; i < lenses.size(); ++i)
                    fprintf(stderr, "  [%zu] layer=%d kind=%g native=%.1f,%.1f %.1fx%.1f r=%.1f a=%.3f\n", i, lenses[i].layer, lenses[i].kind,
                            lenses[i].native[0], lenses[i].native[1], lenses[i].native[2], lenses[i].native[3], lenses[i].radius, lenses[i].alpha);
            }
            // The shader shades a pixel with the first lens covering it, so the knob precedes its track.
            assert(knob >= 0 && track > knob && lensOn(segment(1), 6, 110) < 0);
            auto animations = testClientAnimations.exchange(0);
            shape.SelectedIndex(1);
            PumpEditorFrames();
            assert(lensOn(segment(1), 6, 110) >= 0 && lensOn(segment(0), 6, 110) < 0); // The knob follows the selection.
            shape.SelectedIndex(0);
            PumpEditorFrames();
            testClientAnimations = animations;
            assert(lensOn(segment(0), 6, 110) >= 0);
            ui.scroll.ChangeView(nullptr, 0.0, nullptr, true);
        });
        puts("PASS: a segmented switch lays its segments in a row, its glass knob on the selected one ahead of its track");
        OnEditorAsync([&](SettingsEditor& ui) { ui.navigation.SelectedIndex(3); });
        pumpFor(500); // The page card settles on its new page.
        OnEditorAsync([&](SettingsEditor& ui) {
            ui.hovered = nullptr; // A cursor resting over the window would tint the lenses measured below.
            ui.content.UpdateLayout();
            UpdateEditorSurface(ui, false);
            float scale = GetDpiForWindow(ui.window) / 96.0f;
            using Rect = winrt::Windows::Foundation::Rect;
            auto box = [&](FrameworkElement const& element) {
                assert(element && element.ActualWidth() > 0);
                auto origin = element.TransformToVisual(ui.content).TransformPoint({0, 0});
                return Rect{origin.X, origin.Y, float(element.ActualWidth()), float(element.ActualHeight())};
            };
            // Indices into EditorLenses of glass lenses (kind 2) with exactly this footprint, corner radius and tint opacity.
            auto lensesAt = [&](Rect at, float radius, int alpha) {
                auto lenses = EditorLenses(ui);
                std::vector<uint32_t> found;
                for (uint32_t i = 0; i < lenses.size(); ++i) {
                    auto const* native = lenses[i].native;
                    if (lenses[i].kind == 2 &&
                        std::abs(native[0] - (at.X + at.Width / 2) * scale) < 1 &&
                        std::abs(native[1] - (at.Y + at.Height / 2) * scale) < 1 &&
                        std::abs(native[2] - at.Width * scale / 2) < 1 &&
                        std::abs(native[3] - at.Height * scale / 2) < 1 &&
                        std::abs(lenses[i].radius - radius * scale) < 0.5f &&
                        std::abs(lenses[i].alpha - alpha / 255.0f) < 0.01f) found.push_back(i);
                }
                return found;
            };
            auto glassAt = [&](Rect at, float radius, int alpha) { return int(lensesAt(at, radius, alpha).size()); };
            auto toggle = ui.controls[&SettingDefinition(L"ui.bodyBorder") - kSettingFields].input.as<Controls::ToggleSwitch>();
            auto slider = ui.controls[&SettingDefinition(L"ui.bodyBorderWidth") - kSettingFields].slider;
            auto pill = TestTemplatePart(toggle, L"SwitchKnobBounds"), offPill = TestTemplatePart(toggle, L"OuterBorder");
            auto nativeKnob = TestTemplatePart(toggle, L"SwitchKnob");
            auto thumb = TestTemplatePart(slider, L"HorizontalThumb");
            auto item = ui.navigation.Items().GetAt(3).as<Controls::ListBoxItem>();
            auto pillBox = box(pill), thumbBox = box(thumb);
            float knob = thumbBox.Height, size = pillBox.Height * .6f, inset = (pillBox.Height - size) / 2;
            Rect onKnob{pillBox.X + pillBox.Width - inset - size, pillBox.Y + inset, size, size};
            // Tinted glass rather than a solid fill: Save and the on track carry the accent at 140.
            assert(toggle.IsOn() && glassAt(pillBox, pillBox.Height / 2, 140) == 1);
            assert(glassAt(box(ui.save), 8, 140) == 1 && glassAt(box(ui.reload), 8, 40) == 1);
            // Knobs rest solid. The toggle knob precedes its track, since the shader shades with the first lens hit.
            assert(glassAt({thumbBox.X + thumbBox.Width / 2 - knob / 2, thumbBox.Y, knob, knob}, knob / 2, 255) == 1);
            assert(glassAt(onKnob, size / 2, 255) == 1 &&
                   lensesAt(onKnob, size / 2, 255)[0] < lensesAt(pillBox, pillBox.Height / 2, 140)[0]);
            assert(glassAt(box(item), 10, 110) == 1); // The selected page is a rounded card.
            auto combo = ui.controls[&SettingDefinition(L"ui.bodyBorderColor") - kSettingFields].input.as<Controls::ComboBox>();
            auto number = ui.controls[&SettingDefinition(L"ui.bodyBorderWidth") - kSettingFields].input.as<Controls::TextBox>();
            // Choice and number fields are glass plates too, at the neutral tint and a radius of 8.
            assert(glassAt(box(combo), 8, 40) == 1 && glassAt(box(number), 8, 40) == 1);
            // HasKey also sees theme brushes, so only the control's own entries count. The inbox
            // templates read these state keys when the state changes.
            auto ownAlpha = [&](Controls::Control const& control, PCWSTR key) {
                for (auto const& entry : control.Resources())
                    if (winrt::unbox_value_or<winrt::hstring>(entry.Key(), L"") == key)
                        return int(entry.Value().as<Media::SolidColorBrush>().Color().A);
                return -1;
            };
            int glassHover = ui.content.RequestedTheme() == ElementTheme::Dark ? 31 : 15;
            // The fill a field's template draws at rest; a clear one leaves the plate to the lens.
            auto restFill = [&](Controls::Control const& field, PCWSTR part) {
                return int(TestTemplatePart(field, part).as<Controls::Border>().Background().as<Media::SolidColorBrush>().Color().A);
            };
            auto glassVisuals = [&] {
                return std::array<bool, 11>{bool(pill.Clip()), bool(offPill.Clip()), bool(nativeKnob.Clip()),
                    thumb.Opacity() == 0, bool(item.Style()), ownAlpha(ui.save, L"ButtonBackgroundPointerOver") == 0,
                    ui.save.Background().as<Media::SolidColorBrush>().Color().A == 0,
                    restFill(combo, L"Background") == 0, ownAlpha(combo, L"ComboBoxBackgroundPointerOver") == 0,
                    restFill(number, L"BorderElement") == 0, ownAlpha(number, L"TextControlBackgroundFocused") == 0};
            };
            constexpr std::array<bool, 11> allGlass{true, true, true, true, true, true, true, true, true, true, true};
            assert(glassVisuals() == allGlass);
            Controls::Button untouched; // Glass styling must not recolor the theme brush other buttons share.
            assert(untouched.Resources().Lookup(winrt::box_value(L"ButtonBackgroundPointerOver"))
                .as<Media::SolidColorBrush>().Color().A == 0x33);
            auto contrast = testHighContrast.exchange(1);
            assert(Hosting::ElementCompositionPreview::GetElementChildVisual(ui.contentLayer));
            UpdateEditorSurface(ui);
            assert(!ui.backgroundRenderer); // High contrast keeps the solid controls and native templates.
            assert(!Hosting::ElementCompositionPreview::GetElementChildVisual(ui.contentLayer)); // No plates layer left.
            assert((glassVisuals() == std::array<bool, 11>{}));
            assert(ownAlpha(ui.save, L"ButtonBackgroundPointerOver") == -1 && restFill(combo, L"Background") > 0 &&
                   restFill(number, L"BorderElement") > 0);
            testHighContrast = contrast;
            UpdateEditorSurface(ui);
            assert(ui.backgroundRenderer && glassVisuals() == allGlass &&
                   Hosting::ElementCompositionPreview::GetElementChildVisual(ui.contentLayer));
            puts("PASS: Settings UI glass fields: transparent native fills, glass plates, native again in high contrast");
            // The lens alone shows hover: Save's accent plate takes the neutral overlay, an unselected page a neutral card.
            int hoveredSave = int(std::lround(140 + (255 - 140) * glassHover / 255.0));
            auto other = ui.navigation.Items().GetAt(0).as<FrameworkElement>();
            assert(glassAt(box(ui.save), 8, hoveredSave) == 0 && glassAt(box(other), 10, glassHover) == 0);
            ui.hovered = ui.save;
            UpdateEditorSurface(ui, false);
            assert(glassAt(box(ui.save), 8, hoveredSave) == 1 && glassAt(box(ui.save), 8, 140) == 0);
            ui.hovered = other;
            UpdateEditorSurface(ui, false);
            assert(glassAt(box(other), 10, glassHover) == 1 && glassAt(box(ui.save), 8, 140) == 1);
            ui.hovered = nullptr;
            UpdateEditorSurface(ui, false);
            assert(glassAt(box(other), 10, glassHover) == 0);
            printf("PASS: Settings UI hover lives in the lens: Save alpha 140 -> %d, unselected page card alpha %d\n",
                   hoveredSave, glassHover);
            // EditorChanged places lenses before XAML moves the thumb; the layout pass must move the knob.
            double width = slider.Value();
            slider.Value(width + 5);
            ui.content.UpdateLayout();
            auto moved = box(thumb);
            assert(moved.X > thumbBox.X + 1 && glassAt({moved.X + moved.Width / 2 - knob / 2, moved.Y, knob, knob}, knob / 2, 255) == 1);
            slider.Value(width);
            for (auto const& section : ui.sections) if (section.page == 3) section.header.IsChecked(true);
            ui.scroll.ChangeView(nullptr, 0.0, nullptr, true);
        });
        // A swapchain cannot present in step with a compositor scroll, so plates that scroll live in the content
        // layer, in the scrolled content's own pixels, which the compositor moves with the text. A scroll only
        // moves where their refraction samples the body.
        pumpFor(300);
        LiquidRenderer::LensConstants unscrolled{};
        OnEditorAsync([&](SettingsEditor& ui) {
            assert(ui.scroll.VerticalOffset() == 0 && ui.scroll.ScrollableHeight() > 300);
            std::lock_guard lock(ui.backgroundRenderer->mutex);
            unscrolled = ui.backgroundRenderer->layerLens;
        });
        OnEditorAsync([&](SettingsEditor& ui) { ui.scroll.ChangeView(nullptr, 300.0, nullptr, true); });
        pumpFor(300);
        OnEditorAsync([&](SettingsEditor& ui) {
            float scale = GetDpiForWindow(ui.window) / 96.0f;
            using Rect = winrt::Windows::Foundation::Rect;
            auto boundsIn = [](FrameworkElement const& element, UIElement const& space) {
                return element.TransformToVisual(space).TransformBounds({0, 0, float(element.ActualWidth()), float(element.ActualHeight())});
            };
            assert(std::abs(ui.scroll.VerticalOffset() - 300) < .5);
            auto padding = ui.scroll.Padding();
            auto scroller = boundsIn(ui.scroll, ui.content);
            Rect viewport{scroller.X + float(padding.Left), scroller.Y + float(padding.Top),
                          scroller.Width - float(padding.Left + padding.Right), scroller.Height - float(padding.Top + padding.Bottom)};
            auto inside = [&](float const* native) {
                return native[0] > viewport.X * scale && native[0] < (viewport.X + viewport.Width) * scale &&
                       native[1] > viewport.Y * scale && native[1] < (viewport.Y + viewport.Height) * scale;
            };
            int windowPlates{}, layerPlates{};
            for (auto const& lens : EditorLenses(ui))
                (lens.layer ? layerPlates : windowPlates) += lens.kind == 2 && inside(lens.native);
            LiquidRenderer::LensConstants scrolled{};
            {
                std::lock_guard lock(ui.backgroundRenderer->mutex);
                scrolled = ui.backgroundRenderer->layerLens;
            }
            // The first page-3 text field inside the viewport has its plate at its bounds in the content layer.
            int textPlates = -1;
            for (size_t i = 0; i < ui.controls.size() && textPlates < 0; ++i) {
                auto text = ui.controls[i].input.try_as<Controls::TextBox>();
                if (kSettingFields[i].page != 3 || !text || text.ActualWidth() <= 0) continue;
                auto window = boundsIn(text, ui.content);
                float centre[]{(window.X + window.Width / 2) * scale, (window.Y + window.Height / 2) * scale};
                if (!inside(centre)) continue;
                auto at = boundsIn(text, ui.contentLayer);
                textPlates = 0;
                for (uint32_t j = 0; j < scrolled.lensCount; ++j) {
                    auto const* native = scrolled.lensNativeRects[j];
                    textPlates += scrolled.lensShapeParams[j][3] == 2 && std::abs(native[0] - (at.X + at.Width / 2) * scale) < 1 &&
                        std::abs(native[1] - (at.Y + at.Height / 2) * scale) < 1 && std::abs(native[2] - at.Width * scale / 2) < 1 &&
                        std::abs(native[3] - at.Height * scale / 2) < 1 && std::abs(scrolled.lensRadii[j][0] - 8 * scale) < .5f;
                }
            }
            // Lenses whose centre lies in both scroll positions' drawn bands, before and after, entry for entry. Tints are
            // left out: a cursor resting over the window can hover a control the scroll brought under it.
            auto shared = [&](LiquidRenderer::LensConstants const& lens) {
                std::vector<std::array<float, 20>> entries;
                for (uint32_t j = 0; j < lens.lensCount; ++j) {
                    float y = lens.lensNativeRects[j][1] / scale;
                    if (y < 300 - viewport.Height / 2 + 1 || y > 1.5f * viewport.Height - 1) continue;
                    std::array<float, 20> entry{};
                    for (int k = 0; k < 4; ++k) {
                        entry[k] = lens.lensNativeRects[j][k]; entry[4 + k] = lens.lensRects[j][k];
                        entry[8 + k] = lens.lensRadii[j][k]; entry[12 + k] = lens.lensShapeParams[j][k];
                        entry[16 + k] = lens.lensFades[j][k];
                    }
                    entries.push_back(entry);
                }
                return entries;
            };
            auto before = shared(unscrolled), after = shared(scrolled);
            float shift = scrolled.outputOffset[1] - unscrolled.outputOffset[1], drift{};
            int driftAt = -1;
            for (size_t j = 0; j < std::min(before.size(), after.size()); ++j)
                for (int k = 0; k < 20; ++k)
                    if (std::abs(before[j][k] - after[j][k]) > drift) { drift = std::abs(before[j][k] - after[j][k]); driftAt = int(j * 20 + k); }
            printf("Scrolled 300 DIP: %d window-output plates and %d content-layer plates in the viewport, text field "
                   "plates %d, %zu/%zu shared lenses, largest change %.4f px (entry %d), refraction offset moved %.2f px\n",
                   windowPlates, layerPlates, textPlates, before.size(), after.size(), drift, driftAt, shift);
            assert(windowPlates == 0 && layerPlates > 0); // No open choice list, so the window output holds none.
            assert(textPlates == 1);
            assert(!before.empty() && before.size() == after.size() && drift < .01f);
            assert(std::abs(shift + 300 * scale) < 1 && scrolled.outputOffset[0] == unscrolled.outputOffset[0]);
            for (auto const& section : ui.sections) if (section.page == 3) section.header.IsChecked(false);
            ui.scroll.ChangeView(nullptr, 0.0, nullptr, true);
        });
        puts("PASS: Settings UI scrolled plates live in the content layer: none in the window output's viewport, "
             "a text field at its content bounds, and a scroll leaves their rects unchanged");
        // Samples the page card (the one accent lens at 110) until it reaches the new page and returns how many
        // samples lay strictly between the old and new pages. Each sample is a separate editor task, so frames run between them.
        auto cardSlide = [&](int page, int animations) {
            auto previous = testClientAnimations.exchange(animations);
            float from{}, to{};
            int fromPage{};
            OnEditorAsync([&](SettingsEditor& ui) {
                auto centre = [&](int index) {
                    auto item = ui.navigation.Items().GetAt(index).as<FrameworkElement>();
                    return item.TransformToVisual(ui.content).TransformPoint({0, 0}).Y + float(item.ActualHeight()) / 2;
                };
                fromPage = ui.navigation.SelectedIndex();
                from = centre(fromPage); to = centre(page);
                ui.navigation.SelectedIndex(page);
            });
            assert(std::abs(from - to) > 10);
            int between{};
            for (auto deadline = GetTickCount64() + 2000;;) {
                float y{}; int cards{};
                OnEditorAsync([&](SettingsEditor& ui) {
                    float scale = GetDpiForWindow(ui.window) / 96.0f;
                    std::lock_guard lock(ui.backgroundRenderer->mutex);
                    auto const& lens = ui.backgroundRenderer->lens;
                    for (uint32_t i = 0; i < lens.lensCount; ++i)
                        if (lens.lensShapeParams[i][3] == 2 && std::abs(lens.lensTints[i][3] - 110 / 255.0f) < .01f) {
                            ++cards; y = lens.lensNativeRects[i][1] / scale;
                        }
                });
                assert(cards == 1);
                if (std::abs(y - to) < .5f) break;
                between += (y - from) * (y - to) < 0;
                assert(GetTickCount64() < deadline);
                pumpFor(8);
            }
            pumpFor(100);
            OnEditorAsync([&](SettingsEditor& ui) {
                float scale = GetDpiForWindow(ui.window) / 96.0f;
                auto item = ui.navigation.Items().GetAt(page).as<FrameworkElement>();
                auto origin = item.TransformToVisual(ui.content).TransformPoint({0, 0});
                float expected[]{float(origin.X + item.ActualWidth() / 2) * scale, float(origin.Y + item.ActualHeight() / 2) * scale,
                                 float(item.ActualWidth()) * scale / 2, float(item.ActualHeight()) * scale / 2};
                std::lock_guard lock(ui.backgroundRenderer->mutex);
                auto const& lens = ui.backgroundRenderer->lens;
                int settled{};
                for (uint32_t i = 0; i < lens.lensCount; ++i)
                    settled += lens.lensShapeParams[i][3] == 2 && std::abs(lens.lensTints[i][3] - 110 / 255.0f) < .01f &&
                        std::equal(expected, expected + 4, lens.lensNativeRects[i], [](float a, float b) { return std::abs(a - b) < 1; });
                assert(settled == 1); // The card ends exactly on the selected page.
                assert(!ui.motionSubscribed); // Settled motion releases the per-frame callback.
            });
            testClientAnimations = previous;
            printf("Page card %d -> %d with animations %s: %d samples between the pages\n",
                   fromPage, page, animations ? "on" : "off", between);
            return between;
        };
        assert(cardSlide(0, 1) > 0);
        assert(cardSlide(3, 0) == 0); // Reduced motion moves the card without intermediate frames.
        puts("PASS: Settings UI glass motion: tinted buttons, solid knobs, page card slides");
        std::atomic<int> layouts{};
        FrameworkElement::LayoutUpdated_revoker counting;
        OnEditor([&](SettingsEditor& ui) {
            counting = ui.content.LayoutUpdated(winrt::auto_revoke, [&](auto const&, auto const&) { ++layouts; });
        });
        pumpFor(1500);
        layouts = 0;
        testBackdropTicks = 0;
        pumpFor(1000);
        int idleLayouts = layouts, idleTicks = testBackdropTicks;
        OnEditor([](SettingsEditor& ui) { ui.navigation.SelectedIndex(0); });
        pumpFor(500);
        int pageLayouts = layouts - idleLayouts;
        OnEditor([&](SettingsEditor&) { counting.revoke(); });
        printf("Idle second: %d LayoutUpdated, %d backdrop timer ticks; page switch: %d LayoutUpdated\n",
               idleLayouts, idleTicks, pageLayouts);
        assert(pageLayouts > 0); // The counter sees real layout passes.
        assert(idleLayouts + idleTicks < 10); // Each is one UpdateEditorSurface entry; a feedback loop would run every frame.
        puts("PASS: Settings UI glass controls: card, button, toggle and knob lenses, high-contrast fallback, idle layout");
        assert(LocalInt(ReadLocalSettings(g_settingsPath), L"cornerRadius") == 42);
        auto saved = ReadLocalFile(g_settingsPath);
        auto generation = g_configurationGeneration;
        OnEditor([](SettingsEditor& ui) { InvokeSave(ui); });
        WaitFor([&] { return ReadLocalFile(g_settingsPath) == saved; });
        OnEditor([](SettingsEditor& ui) {
            ui.controls[3].input.as<Controls::TextBox>().Text(L"9999");
            InvokeSave(ui);
        });
        assert(ReadLocalFile(g_settingsPath) == saved);
        OnEditor([](SettingsEditor& ui) { ui.controls[3].input.as<Controls::TextBox>().Text(L"43"); });
        std::string external = "schemaVersion=1\n[settings]\ncornerRadius=66\n";
        assert(WriteLocalFile(g_settingsPath, external, true));
        SendMessageW(editor, WM_COMMAND, kEditorSave, 0);
        assert(ReadLocalFile(g_settingsPath) == external);
        SendMessageW(editor, WM_COMMAND, kEditorReload, 0);
        OnEditor([](SettingsEditor& ui) { assert(ui.controls[3].input.as<Controls::TextBox>().Text() == L"66"); });
        SendMessageW(editor, WM_COMMAND, kEditorSave, 0);
        WaitFor([] { return g_dockCornerRadius == 66; });
        assert(g_configurationGeneration > generation);
        assert(DeleteFileW(g_settingsPath.c_str()));
        SendMessageW(editor, WM_COMMAND, kEditorReload, 0);
        SendMessageW(editor, WM_COMMAND, kEditorSave, 0);
        WaitFor([] { return g_dockCornerRadius == 12; });
        assert(ReadLocalFile(g_settingsPath)); // Save recreates a missing file.
        auto segmented = [](SettingsEditor& ui, PCWSTR key) {
            return ui.controls[&SettingDefinition(key) - kSettingFields].input.as<Controls::ListBox>();
        };
        auto innerArcs = [](SettingsEditor& ui) {
            return ui.controls[&SettingDefinition(L"innerArcs") - kSettingFields].input.as<Controls::ToggleSwitch>();
        };
        auto smallerTray = [](SettingsEditor& ui) {
            return ui.controls[&SettingDefinition(L"smallerTray") - kSettingFields].input.as<Controls::ToggleSwitch>();
        };
        auto reduction = [](SettingsEditor& ui) -> EditorControl& { return ui.controls[&SettingDefinition(L"trayReduction") - kSettingFields]; };
        auto slantedEnds = [](SettingsEditor& ui) {
            return ui.controls[&SettingDefinition(L"slantedEnds") - kSettingFields].input.as<Controls::ToggleSwitch>();
        };
        auto curveAngle = [](SettingsEditor& ui) -> EditorControl& { return ui.controls[&SettingDefinition(L"slantAngle") - kSettingFields]; };
        OnEditor([&](SettingsEditor& ui) {
            auto shape = segmented(ui, L"shape"), alignment = segmented(ui, L"alignment"), layout = segmented(ui, L"layout");
            auto arcs = innerArcs(ui);
            assert(shape.Items().Size() == 3 && shape.SelectedIndex() == 0 && alignment.SelectedIndex() == 0 &&
                   layout.SelectedIndex() == 0 && ui.controls[6].input.IsEnabled() && !arcs.IsOn() && !arcs.IsEnabled());
            // Slanted ends, its Curve angle and its Concave radius sit between Arc the inner edges and Smaller tray. Slanted
            // ends is off by default and enabled whatever the shape; the angle (30..85, default 70) and the concave radius
            // (0..100, default 18) are enabled only with it on.
            auto card = [&](PCWSTR key) { return ui.controls[&SettingDefinition(key) - kSettingFields].card; };
            auto rows = card(L"slantedEnds").Parent().as<Controls::Panel>().Children();
            uint32_t arcsAt{}, slantAt{}, angleAt{}, concaveAt{}, trayAt{};
            assert(rows.IndexOf(card(L"innerArcs"), arcsAt) && rows.IndexOf(card(L"slantedEnds"), slantAt) &&
                   rows.IndexOf(card(L"slantAngle"), angleAt) && rows.IndexOf(card(L"concaveRadius"), concaveAt) &&
                   rows.IndexOf(card(L"smallerTray"), trayAt) && slantAt == arcsAt + 1 && angleAt == slantAt + 1 &&
                   concaveAt == angleAt + 1 && trayAt == concaveAt + 1);
            assert(!slantedEnds(ui).IsOn() && slantedEnds(ui).IsEnabled());
            auto& curve = curveAngle(ui);
            auto& concave = ui.controls[&SettingDefinition(L"concaveRadius") - kSettingFields];
            assert(!curve.input.IsEnabled() && !curve.slider.IsEnabled() && curve.slider.Value() == 70 &&
                   curve.slider.Minimum() == 30 && curve.slider.Maximum() == 85);
            assert(!concave.input.IsEnabled() && !concave.slider.IsEnabled() && concave.slider.Value() == 18 &&
                   concave.slider.Minimum() == 0 && concave.slider.Maximum() == 100);
            slantedEnds(ui).IsOn(true);
            assert(curve.input.IsEnabled() && curve.slider.IsEnabled() && concave.input.IsEnabled() && concave.slider.IsEnabled());
            curve.slider.Value(60);
            concave.slider.Value(30);
            assert(curve.input.as<Controls::TextBox>().Text() == L"60" && concave.input.as<Controls::TextBox>().Text() == L"30");
            slantedEnds(ui).IsOn(false);
            assert(!curve.input.IsEnabled() && !curve.slider.IsEnabled() && !concave.input.IsEnabled());
            // Tray height reduction is enabled only with Smaller tray on.
            auto& amount = reduction(ui);
            assert(!smallerTray(ui).IsOn() && !amount.input.IsEnabled() && !amount.slider.IsEnabled() &&
                   amount.slider.Value() == 8 && amount.slider.Minimum() == 1 && amount.slider.Maximum() == 16);
            smallerTray(ui).IsOn(true);
            assert(amount.input.IsEnabled() && amount.slider.IsEnabled());
            amount.slider.Value(12);
            assert(amount.input.as<Controls::TextBox>().Text() == L"12");
            shape.SelectedIndex(1);
            alignment.SelectedIndex(1);
            layout.SelectedIndex(1);
            assert(!ui.controls[6].input.IsEnabled()); // Edge has no distance from the screen edge.
            assert(!arcs.IsEnabled()); // Edge has no arcs.
            assert(slantedEnds(ui).IsEnabled());
            shape.SelectedIndex(2);
            assert(!ui.controls[6].input.IsEnabled() && arcs.IsEnabled()); // Separate Edge arc has facing flares.
            assert(slantedEnds(ui).IsEnabled());
            auto& distance = ui.controls[&SettingDefinition(L"barDistance") - kSettingFields];
            assert(distance.input.IsEnabled() && distance.slider.Value() == 12); // Separate parts the bars by Bar distance.
            layout.SelectedIndex(0);
            assert(!arcs.IsEnabled()); // One bar has no facing walls,
            assert(!distance.input.IsEnabled()); // and no distance between bars.
            assert(slantedEnds(ui).IsEnabled());
            layout.SelectedIndex(1);
            arcs.IsOn(true);
            slantedEnds(ui).IsOn(true);
            shape.SelectedIndex(-1);
            assert(shape.SelectedIndex() == 2); // A switch keeps one segment on.
            InvokeSave(ui);
        });
        WaitFor([] { return g_dockStyle.shape == DockShape::EdgeArc; });
        auto styled = ReadLocalSettings(g_settingsPath);
        assert(LocalString(styled, L"shape") == L"edgeArc" && LocalString(styled, L"alignment") == L"buttons" &&
               LocalString(styled, L"layout") == L"separate" && LocalBool(styled, L"innerArcs"));
        assert(g_dockStyle.alignment == DockAlignment::Buttons && g_dockStyle.layout == DockLayout::Separate && g_dockStyle.innerArcs);
        assert(g_dockEdgeDistance == 0 && LocalInt(styled, L"edgeDistance") == 4); // Applied, while the file keeps the distance.
        assert(LocalBool(styled, L"smallerTray") && LocalInt(styled, L"trayReduction") == 12 && g_dockStyle.trayReduction == 12);
        // A 60-degree middle leans the walls 30 degrees from upright, and the concave curves take the saved radius.
        assert(LocalBool(styled, L"slantedEnds") && LocalInt(styled, L"slantAngle") == 60 && g_dockStyle.slant == 30);
        assert(LocalInt(styled, L"concaveRadius") == 30 && g_dockStyle.concave == 30);
        OnEditor([&](SettingsEditor& ui) {
            segmented(ui, L"shape").SelectedIndex(0);
            segmented(ui, L"alignment").SelectedIndex(0);
            segmented(ui, L"layout").SelectedIndex(0);
            innerArcs(ui).IsOn(false);
            smallerTray(ui).IsOn(false);
            slantedEnds(ui).IsOn(false);
            reduction(ui).slider.Value(3);
            curveAngle(ui).slider.Value(40);
        });
        SendMessageW(editor, WM_COMMAND, kEditorReload, 0);
        OnEditor([&](SettingsEditor& ui) {
            assert(segmented(ui, L"shape").SelectedIndex() == 2 && segmented(ui, L"alignment").SelectedIndex() == 1 &&
                   segmented(ui, L"layout").SelectedIndex() == 1 && innerArcs(ui).IsOn() && innerArcs(ui).IsEnabled());
            assert(smallerTray(ui).IsOn() && reduction(ui).slider.Value() == 12 && reduction(ui).input.IsEnabled());
            assert(slantedEnds(ui).IsOn() && slantedEnds(ui).IsEnabled() && curveAngle(ui).slider.Value() == 60 &&
                   curveAngle(ui).input.IsEnabled());
            segmented(ui, L"shape").SelectedIndex(0);
            segmented(ui, L"alignment").SelectedIndex(0);
            segmented(ui, L"layout").SelectedIndex(0);
            innerArcs(ui).IsOn(false);
            smallerTray(ui).IsOn(false);
            slantedEnds(ui).IsOn(false);
            assert(!reduction(ui).input.IsEnabled() && slantedEnds(ui).IsEnabled() && !curveAngle(ui).input.IsEnabled());
            InvokeSave(ui);
        });
        WaitFor([] { return g_dockStyle.shape == DockShape::Island && g_dockStyle.alignment == DockAlignment::Group; });
        assert(g_dockStyle.layout == DockLayout::Unified && !g_dockStyle.innerArcs && g_dockEdgeDistance == 4);
        // Smaller tray off applies no reduction, while the file keeps the slider's value.
        styled = ReadLocalSettings(g_settingsPath);
        assert(!LocalBool(styled, L"smallerTray") && LocalInt(styled, L"trayReduction") == 12 && g_dockStyle.trayReduction == 0);
        assert(!LocalBool(styled, L"slantedEnds") && LocalInt(styled, L"slantAngle") == 60 && g_dockStyle.slant == 0);
        OnEditor([&](SettingsEditor& ui) {
            // One visual carries one clip, so Edge's silhouette is a second geometry bound to the hull. The
            // highlights, the border line and the Liquid shadow must draw that silhouette, not the hull.
            Controls::Canvas host;
            host.Width(640); host.Height(80);
            Controls::Grid background, parts;
            background.Width(640); background.Height(80);
            Shapes::Rectangle fill, stroke;
            fill.Name(L"BackgroundFill"); stroke.Name(L"BackgroundStroke");
            parts.Children().Append(fill); parts.Children().Append(stroke);
            background.Children().Append(parts); host.Children().Append(background);
            ui.preview.canvas.Children().Append(host);
            host.Measure({640, 80}); host.Arrange({0, 0, 640, 80}); ui.content.UpdateLayout();
            auto original = Hosting::ElementCompositionPreview::GetElementVisual(background).Clip();
            for (auto shape : {DockShape::Island, DockShape::Edge}) {
                Root root;
                root.frame = host;
                g_dockStyle.shape = shape;
                auto shapeOutline = MakeOutline(g_dockStyle, 150, 12, 72, false, kContent);
                assert(PrepareDock(root.dock, background, {4, 4}, shapeOutline));
                auto& dock = root.dock;
                assert(dock.clip.Geometry() == dock.outline && dock.visual.Clip() == dock.clip);
                assert((dock.outline == dock.geometry) == (shape == DockShape::Island));
                assert(PrepareDock(root.dock, background, {4, 4}, shapeOutline)); // Idempotent: unchanged ownership.
                auto outline = dock.outline.as<CompositionRoundedRectangleGeometry>();
                assert(outline.CornerRadius().x == 12 && dock.geometry.Size().y == 72);
                auto highlights = GlassHighlights(dock.outline, DockTheme::Glass, false, DockBorder{{255, 0, 0, 0}, 2});
                assert(highlights.Shapes().Size() == 2);
                for (uint32_t i = 0; i < highlights.Shapes().Size(); ++i)
                    assert(highlights.Shapes().GetAt(i).as<CompositionSpriteShape>().Geometry() == dock.outline);
                GlassShadow shadow(ui.preview.canvas, host, dock.outline);
                assert(shadow.mask.Shapes().GetAt(0).as<CompositionSpriteShape>().Geometry() == dock.outline);
                // A clip taken over elsewhere is not ours to drive any more.
                auto taken = dock.visual.Compositor().CreateGeometricClip(dock.geometry);
                dock.visual.Clip(taken);
                assert(!PrepareDock(root.dock, background, {4, 4}, shapeOutline) && !RootLayoutUnchanged(root));
                dock.visual.Clip(dock.clip);
                RestoreDock(root);
                assert(!dock.outline && !dock.geometry && dock.visual == nullptr);
                assert(Hosting::ElementCompositionPreview::GetElementVisual(background).Clip() == original);
            }
            g_dockStyle.shape = DockShape::Island;
            uint32_t index{};
            assert(ui.preview.canvas.Children().IndexOf(host, index));
            ui.preview.canvas.Children().RemoveAt(index);
        });
        puts("PASS: Edge adds a second clip geometry that the highlights, border line and shadow draw, and restore releases both");
        puts("PASS: Layout, Shape (three segments) and Alignment switches and Arc the inner edges save, apply and reload; "
             "Edge and Edge arc disable the edge distance, and Arc the inner edges is enabled only for Separate Edge arc");
        puts("PASS: Smaller tray and Tray height reduction save, apply and reload; the reduction (1..16, default 8) is "
             "enabled only with Smaller tray on, and Smaller tray off applies none while the file keeps the value");
        puts("PASS: Slanted ends and Curve angle sit between Arc the inner edges and Smaller tray; Slanted ends is off by "
             "default and enabled for every shape and layout, Curve angle (30..85, default 70) is enabled only with it on, and "
             "a 60-degree angle saves, applies a 30-degree lean, reloads, and stays in the file with Slanted ends saved off");
        // 0.4.5: the slider reaches the editor's taskbar band less 24 DIP and never drops below the saved value, so the
        // editor keeps a reduction only a taller taskbar uses in full.
        auto band = [](double height) { TaskbarPreviewMetrics metrics; metrics.dockHeight = height; return metrics; };
        auto reductionText = [&](SettingsEditor& ui) { return std::wstring(reduction(ui).input.as<Controls::TextBox>().Text()); };
        OnEditor([&](SettingsEditor& ui) {
            auto slider = reduction(ui).slider;
            ApplyPreviewMetrics(ui, band(68));
            double tall = slider.Maximum();
            ApplyPreviewMetrics(ui, band(67.2));
            double fractional = slider.Maximum();
            ApplyPreviewMetrics(ui, {});
            printf("TRAY-REDUCTION-MAX stored %d: band 68 -> %g, band 67.2 -> %g, band 40 -> %g\n",
                   LocalInt(ui.savedValues, L"trayReduction"), tall, fractional, slider.Maximum());
            assert(tall == 44 && fractional == 43 && slider.Maximum() == 16 && slider.Value() == 12 && !ui.dirty);
        });
        OnEditor([&](SettingsEditor& ui) {
            auto& amount = reduction(ui);
            smallerTray(ui).IsOn(true);
            amount.input.as<Controls::TextBox>().Text(L"30"); // Above this band's 16: the field takes the row's 1..100.
            double typed = amount.slider.Value();
            InvokeSave(ui);
            printf("TRAY-REDUCTION typed 30 at band 40: slider %g, after save %g of %g\n", typed, amount.slider.Value(),
                   amount.slider.Maximum());
            assert(amount.slider.Maximum() == 30 && !ui.dirty);
        });
        WaitFor([] { return g_dockStyle.trayReduction == 30; }); // Applied whole; each taskbar's TrayDrop clamps it.
        assert(LocalInt(ReadLocalSettings(g_settingsPath), L"trayReduction") == 30);
        SendMessageW(editor, WM_COMMAND, kEditorReload, 0);
        auto storedFile = ReadLocalFile(g_settingsPath);
        OnEditor([&](SettingsEditor& ui) {
            auto& amount = reduction(ui);
            auto keeps30 = [&] {
                return amount.slider.Maximum() == 30 && amount.slider.Value() == 30 && reductionText(ui) == L"30" && !ui.dirty;
            };
            assert(keeps30());
            ApplyPreviewMetrics(ui, band(68));
            assert(amount.slider.Maximum() == 44 && amount.slider.Value() == 30 && !ui.dirty);
            ApplyPreviewMetrics(ui, {});
            assert(keeps30());
            InvokeSave(ui);
        });
        assert(ReadLocalFile(g_settingsPath) == storedFile); // Saving untouched writes the 30 back unchanged.
        SendMessageW(editor, WM_COMMAND, kEditorReload, 0);
        OnEditor([&](SettingsEditor& ui) {
            auto& amount = reduction(ui);
            assert(amount.slider.Maximum() == 30 && amount.slider.Value() == 30 && reductionText(ui) == L"30" && !ui.dirty);
            // An unsaved drag on a taller taskbar, then the editor moves to a 48 DIP one: the slider's range shrinks to
            // the saved 30, and the 40 beside it is not rewritten.
            ApplyPreviewMetrics(ui, band(68));
            amount.slider.Value(40);
            assert(reductionText(ui) == L"40" && ui.dirty);
            ApplyPreviewMetrics(ui, {});
            double lowered = amount.slider.Value(), maximum = amount.slider.Maximum();
            auto text = reductionText(ui);
            ApplyPreviewMetrics(ui, band(68));
            printf("TRAY-REDUCTION unsaved 40 at band 68, then band 40: slider %g of %g, text %ls; back at band 68: slider %g\n",
                   lowered, maximum, text.c_str(), amount.slider.Value());
            assert(maximum == 30 && lowered == 30 && text == L"40");
            ApplyPreviewMetrics(ui, {});
            smallerTray(ui).IsOn(false);
            assert(!amount.input.IsEnabled() && !amount.slider.IsEnabled()); // Off still disables the row.
            amount.input.as<Controls::TextBox>().Text(L"12");
            InvokeSave(ui);
            assert(amount.slider.Maximum() == 16);
        });
        WaitFor([] { return g_dockStyle.trayReduction == 0; });
        styled = ReadLocalSettings(g_settingsPath);
        assert(!LocalBool(styled, L"smallerTray") && LocalInt(styled, L"trayReduction") == 12);
        puts("PASS: Tray height reduction's slider reaches the band less 24 DIP (16 at 40, 44 at 68, 43 at 67.2) and never "
             "drops below the saved value: a typed 30 saves and lifts it to 30, a stored 30 at band 40 keeps 30 and saves and "
             "reloads unchanged and clean, a band change does not rewrite an unsaved value, and Smaller tray off disables the row");
        OnEditorAsync(CheckSeparateMorph);
        OnEditorAsync(CheckDockLead);
        OnEditorAsync(CheckTrayBand);
        OnEditorAsync(CheckResizeFrame);
        std::shared_ptr<LiquidRenderer> previewRenderer;
        OnEditorAsync([&](SettingsEditor& ui) {
            ui.controls[0].input.as<Controls::ComboBox>().SelectedIndex(0);
            assert(!ui.controls[9].input.IsEnabled() && !ui.controls[10].input.IsEnabled());
            assert(ui.trayEnabled.IsEnabled()); // Master tray toggle is independent of dock material.
            ui.controls[0].input.as<Controls::ComboBox>().SelectedIndex(3);
            ui.controls[9].input.as<Controls::ComboBox>().SelectedIndex(0);
            assert(ui.controls[10].input.IsEnabled());
            ui.navigation.SelectedIndex(1);
            assert(ui.pages[0].Visibility() == Visibility::Collapsed && ui.pages[1].Visibility() == Visibility::Visible);
            ui.controls[17].slider.Value(-12);
            assert(ui.controls[17].input.as<Controls::TextBox>().Text() == L"-12");
            ui.controls[6].slider.Value(24);
            assert(Controls::Canvas::GetTop(ui.preview.material) == (ui.preview.height - 40) / 2.0);
            assert(!ui.preview.unavailable && ui.preview.renderer);
            previewRenderer = ui.preview.renderer;
            ui.controls[11].slider.Value(0);
            std::lock_guard lock(previewRenderer->mutex);
            assert(previewRenderer->taskbar == nullptr && previewRenderer->sources.size() == 1);
            assert(!previewRenderer->sources[0]->session && !previewRenderer->sources[0]->pool);
            assert(previewRenderer->lens.optics.refraction == 0);
        });
        auto previewIdle = [&] {
            std::lock_guard lock(previewRenderer->mutex);
            return !previewRenderer->pendingPresent && !previewRenderer->presentWorkPending;
        };
        WaitFor(previewIdle);
        auto flatPixels = PreviewPixels(previewRenderer);
        auto beforePreview = ReadLocalFile(g_settingsPath);
        OnEditor([](SettingsEditor& ui) { ui.controls[11].slider.Value(200); });
        WaitFor(previewIdle);
        assert(PreviewPixels(previewRenderer) != flatPixels);
        assert(ReadLocalFile(g_settingsPath) == beforePreview && g_dockCornerRadius == 12);
        OnEditor([](SettingsEditor& ui) {
            ui.controls[8].input.as<Controls::ToggleSwitch>().IsOn(true);
            std::lock_guard lock(ui.preview.renderer->mutex);
            assert(ui.preview.renderer->lens.lensCount==ui.preview.appIcons.size());
        });
        previewRenderer.reset();
        puts("PASS: draft preview geometry, GPU refraction, button lenses, no capture, and no persisted/live changes");
        OnEditor([](SettingsEditor& ui) {
            Controls::Grid tree;
            tree.Width(100); tree.Height(100);
            Controls::Image icon, badge;
            icon.Source(ui.preview.appIcons[0].Source());
            badge.Source(ui.preview.appIcons[1].Source());
            icon.Width(32); icon.Height(24); icon.Stretch(Media::Stretch::Fill);
            badge.Width(8); badge.Height(8);
            Media::ScaleTransform scale;
            scale.ScaleX(1.5); scale.ScaleY(1.5);
            icon.RenderTransform(scale);
            tree.Children().Append(icon); tree.Children().Append(badge);
            ui.preview.canvas.Children().Append(tree);
            ui.content.UpdateLayout();
            tree.Measure({100, 100}); tree.Arrange({0, 0, 100, 100});
            auto size = LargestAppIconBox(tree, tree);
            if (size.Width != 48 || size.Height != 36)
                fwprintf(stderr, L"Icon box %.2f x %.2f, layout %.2f x %.2f\n",
                         size.Width, size.Height, icon.ActualWidth(), icon.ActualHeight());
            assert(size.Width == 48 && size.Height == 36);
            tree.Visibility(Visibility::Collapsed);
            assert(LargestAppIconBox(tree, tree).Width == 0);
            uint32_t treeIndex{};
            assert(ui.preview.canvas.Children().IndexOf(tree, treeIndex));
            ui.preview.canvas.Children().RemoveAt(treeIndex);
            TaskbarPreviewMetrics metrics;
            metrics.frameHeight = 64; metrics.dockHeight = 56;
            metrics.iconWidth = metrics.iconHeight = 32;
            metrics.iconMin = 24; metrics.iconMax = 32;
            metrics.dpi = 144; metrics.frameMeasured = metrics.iconMeasured = true;
            ApplyPreviewMetrics(ui, metrics);
            assert(ui.preview.appIcons[0].Width() == 32 && ui.preview.material.Height() == 56);
            assert(std::wstring_view(ui.preview.measurement.Text()).find(L"96 px") != std::wstring_view::npos);
            double centeredTop = (ui.preview.height - metrics.dockHeight) / 2.0;
            assert(Controls::Canvas::GetTop(ui.preview.material) == centeredTop);
            for (int edge : {0, 100}) {
                ui.controls[6].slider.Value(edge);
                assert(Controls::Canvas::GetTop(ui.preview.material) == centeredTop);
                std::lock_guard lock(ui.preview.renderer->mutex);
                assert(ui.preview.renderer->lens.center[1] == ui.preview.height / 2.0f);
            }
            float middle = float(48 + ui.preview.material.Width() / 2), half = float(metrics.dockHeight) / 2;
            // Island runs the hull's own rounded-rect distance: half the band deep at the dock's centre,
            // and 5 px outside 5 px above its top edge.
            assert(std::abs(DockProbe(ui.preview.renderer, middle, float(centeredTop) + half)[0] + half) < .01f);
            assert(std::abs(DockProbe(ui.preview.renderer, middle, float(centeredTop) - 5)[0] - 5) < .01f);
            // Edge sits on the canvas bottom like a bottom taskbar: square there, round at the top, its shader
            // outline carried past the last row while the optics keep the visible band.
            auto shape = ui.controls[&SettingDefinition(L"shape") - kSettingFields].input.as<Controls::ListBox>();
            shape.SelectedIndex(1);
            double edgeTop = ui.preview.height - metrics.dockHeight;
            auto corners = ui.preview.material.CornerRadius();
            assert(Controls::Canvas::GetTop(ui.preview.material) == edgeTop);
            assert(corners.TopLeft == 12 && corners.TopRight == 12 && corners.BottomRight == 0 && corners.BottomLeft == 0);
            {
                std::lock_guard lock(ui.preview.renderer->mutex);
                assert(ui.preview.renderer->lens.center[1] == edgeTop + metrics.dockHeight / 2 &&
                       ui.preview.renderer->lens.halfSize[1] == metrics.dockHeight / 2);
            }
            float lastRow = ui.preview.height - .5f, nearLeft = 49.5f;
            assert(DockProbe(ui.preview.renderer, middle, lastRow)[1] == 1);
            assert(DockProbe(ui.preview.renderer, nearLeft, lastRow)[1] == 1); // A square corner on the edge.
            assert(DockProbe(ui.preview.renderer, nearLeft, float(edgeTop) + .5f)[1] == 0); // A round far corner.
            // Edge arc flares out along the bottom: the material widens by the radius on each side while the lens
            // keeps the band, and the shader covers each flare's foot on the last row but not the disc it curves around.
            float dockLeft = 48, dockRight = float(48 + ui.preview.material.Width()), disc = float(ui.preview.height) - 11;
            shape.SelectedIndex(2);
            assert(Controls::Canvas::GetLeft(ui.preview.material) == dockLeft - 12 &&
                   ui.preview.material.Width() == dockRight - dockLeft + 24 && Controls::Canvas::GetTop(ui.preview.material) == edgeTop);
            {
                std::lock_guard lock(ui.preview.renderer->mutex);
                assert(ui.preview.renderer->lens.center[0] == (dockLeft + dockRight) / 2 &&
                       ui.preview.renderer->lens.halfSize[0] == (dockRight - dockLeft) / 2);
            }
            // The foot sits about a pixel inside the arc on this row, where the preview's coverage ramp is past half.
            assert(DockProbe(ui.preview.renderer, dockLeft - 6, lastRow)[1] > .5f && DockProbe(ui.preview.renderer, dockRight + 6, lastRow)[1] > .5f);
            assert(DockProbe(ui.preview.renderer, dockLeft - 11, disc)[1] == 0 && DockProbe(ui.preview.renderer, dockRight + 11, disc)[1] == 0);
            assert(DockProbe(ui.preview.renderer, nearLeft, float(edgeTop) + .5f)[1] == 0);
            shape.SelectedIndex(0);
            assert(Controls::Canvas::GetLeft(ui.preview.material) == dockLeft && ui.preview.material.Width() == dockRight - dockLeft);
            assert(Controls::Canvas::GetTop(ui.preview.material) == centeredTop);
            assert(DockProbe(ui.preview.renderer, middle, lastRow)[1] == 0);
            metrics.iconWidth = 40;
            metrics.iconMin = metrics.iconMax = metrics.iconHeight;
            ApplyPreviewMetrics(ui, metrics);
            assert(std::wstring_view(ui.preview.measurement.Text()).find(L"60×48 px") != std::wstring_view::npos);
            ApplyPreviewMetrics(ui, {});
            assert(std::wstring_view(ui.preview.measurement.Text()).find(L"estimate") != std::wstring_view::npos);
        });
        puts("PASS: real icon assets, transformed icon boxes, DPI reporting, centered preview independent of screen-edge distance, and estimates");
        puts("PASS: Island keeps the hull's own shader distance; the Edge preview sits on the bottom edge, square there "
             "and round above, its shader outline covering the last row");
        puts("PASS: the Liquid Edge arc preview widens the material by its flares, keeps the lens on the band, and its "
             "shader covers each flare's foot on the last row but not the flare's disc");
        OnEditorAsync(CheckSeparatePreview);
        OnEditorAsync(CheckLiquidClose);
        puts("PASS: liquid close keeps the native animated footprint despite early target geometry; preview retains its shader mask");
        OnEditor([&](SettingsEditor& ui) {
            ui.navigation.SelectedIndex(2);
            assert(ui.pages[2].Visibility() == Visibility::Visible);
            assert(ui.controls[2].input.IsEnabled()); // Independent of text color mode.
            ui.catalog = {{L"123", L"Sample app", LoadTrayImage(customFile), false},
                          {L"456", L"Overflow app", {}, true}};
            RebuildTrayRows(ui);
            assert(ui.trayRows.size() == 2);
            ui.trayOverrides[L"123"] = {customFile, customFile};
            EditorChanged(ui); RebuildTrayRows(ui);
            assert(ui.trayRows[0].reset.IsEnabled() && !ui.trayRows[1].reset.IsEnabled());
            InvokeSave(ui);
        });
        WaitFor([&] { return g_customTrayIcons.contains(L"123"); });
        assert(ReadTrayOverrides(ReadLocalSettings(g_settingsPath)).at(L"123").light == customFile);
        OnEditor([](SettingsEditor& ui) {
            assert(ui.trayEnabled.IsOn());
            ui.controls[2].input.as<Controls::ToggleSwitch>().IsOn(true);
            ui.trayEnabled.IsOn(false);
            assert(!ui.controls[2].input.IsEnabled() && !ui.trayRows[0].choose.IsEnabled());
            assert(!ui.trayRows[0].reset.IsEnabled() && ui.trayOverrides.contains(L"123"));
            InvokeSave(ui);
        });
        WaitFor([] { return g_customTrayIcons.empty() && !g_grayscaleTrayIcons; });
        auto disabledTray = ReadLocalSettings(g_settingsPath);
        assert(!LocalBool(disabledTray, L"tray.enabled") && LocalBool(disabledTray, L"tray.grayscale"));
        assert(ReadTrayOverrides(disabledTray).at(L"123").light == customFile);
        OnEditor([](SettingsEditor& ui) {
            ReloadEditor(ui);
            assert(!ui.trayEnabled.IsOn() && ui.trayOverrides.contains(L"123"));
            assert(ui.controls[2].input.as<Controls::ToggleSwitch>().IsOn() && !ui.controls[2].input.IsEnabled());
            ui.trayEnabled.IsOn(true);
            assert(ui.controls[2].input.IsEnabled() && ui.trayRows[0].choose.IsEnabled());
            InvokeSave(ui);
        });
        WaitFor([] { return g_customTrayIcons.contains(L"123") && g_grayscaleTrayIcons; });
        assert(LocalBool(ReadLocalSettings(g_settingsPath), L"tray.enabled"));
        OnEditor([](SettingsEditor& ui) {
            ui.controls[2].input.as<Controls::ToggleSwitch>().IsOn(false);
            Automation::Peers::ButtonAutomationPeer peer(ui.trayRows[0].reset);
            peer.GetPattern(Automation::Peers::PatternInterface::Invoke)
                .as<Automation::Provider::IInvokeProvider>().Invoke();
            PumpEditorFrames();
            assert(!ui.trayOverrides.contains(L"123"));
            InvokeSave(ui);
        });
        WaitFor([] { return g_customTrayIcons.empty(); });
        assert(ReadTrayOverrides(ReadLocalSettings(g_settingsPath)).empty());
        puts("PASS: tray master toggle saves/reloads, disables both effects, and preserves assignments and grayscale choice");
        puts("PASS: tray page, per-icon assignment, save/apply, and Use original");
        // The preview's app artwork reads the master tray toggle the way the runtime does: no grayscale while
        // customization is off, whatever the stored grayscale choice says. Async: the shown editor pumps frames,
        // which needs the main thread's loop running.
        OnEditorAsync([](SettingsEditor& ui) {
            int page = ui.navigation.SelectedIndex();
            bool dirty = ui.dirty;
            ui.controls[2].input.as<Controls::ToggleSwitch>().IsOn(true);
            ui.trayEnabled.IsOn(false);
            {
                ShownEditor shown(ui);
                UpdateEditorPreview(ui);
                assert(ui.preview.icons[0].Foreground().as<Media::SolidColorBrush>().Color().R == 0x3C);
            }
            ui.trayEnabled.IsOn(true);
            ui.controls[2].input.as<Controls::ToggleSwitch>().IsOn(false);
            ui.navigation.SelectedIndex(page);
            ui.dirty = dirty;
        });
        puts("PASS: the preview's tray artwork follows the master toggle like the runtime");
        // Preview Liquid glass changes the draft theme and nothing else; on Liquid the same button reads
        // Use custom controls and selects the Custom preset.
        OnEditorAsync([](SettingsEditor& ui) {
            auto theme = ui.controls[0].input.as<Controls::ComboBox>(), preset = ui.controls[9].input.as<Controls::ComboBox>();
            int savedTheme = theme.SelectedIndex(), savedPreset = preset.SelectedIndex(), page = ui.navigation.SelectedIndex();
            bool dirty = ui.dirty;
            ui.navigation.SelectedIndex(1);
            theme.SelectedIndex(int(DockTheme::Glass)); preset.SelectedIndex(2);
            auto click = [&] {
                Automation::Peers::ButtonAutomationPeer(ui.enableLiquid).GetPattern(Automation::Peers::PatternInterface::Invoke)
                    .as<Automation::Provider::IInvokeProvider>().Invoke();
            };
            click();
            assert(theme.SelectedIndex() == int(DockTheme::Liquid) && preset.SelectedIndex() == 2);
            click();
            assert(preset.SelectedIndex() == 0);
            theme.SelectedIndex(savedTheme); preset.SelectedIndex(savedPreset);
            ui.navigation.SelectedIndex(page);
            ui.dirty = dirty;
        });
        puts("PASS: Preview Liquid glass keeps the saved preset; Use custom controls selects Custom");
        // A half-DIP field rejects a quarter step and names the step it takes.
        OnEditorAsync([](SettingsEditor& ui) {
            size_t border = std::find_if(std::begin(kSettingFields), std::end(kSettingFields),
                [](auto const& field) { return std::wstring_view(field.key) == L"dockBorderWidth"; }) - kSettingFields;
            assert(kSettingFields[border].displayDivisor == 2);
            auto box = ui.controls[border].input.as<Controls::TextBox>();
            auto text = box.Text();
            ui.syncing = true;
            box.Text(L"1.25");
            auto message = EditorFieldError(ui.controls[border], kSettingFields[border]);
            assert(message.find(L"steps of 0.50") != std::wstring::npos);
            int value{};
            box.Text(L"1.5");
            assert(EditorNumber(box, kSettingFields[border], value) && value == 3);
            box.Text(text);
            ui.syncing = false;
        });
        puts("PASS: a half-DIP border rejects 1.25 naming the 0.50 step and accepts 1.5");
        OnEditor([](SettingsEditor& ui) {
            OpenCatalogPicker(ui, L"123");
            assert(ui.iconPicker.open && !ui.content.IsEnabled());
        });
        WaitFor([] {
            bool loaded{};
            OnEditor([&](SettingsEditor& ui) {
                auto& picker = ui.iconPicker;
                loaded = picker.entries.size() == 3 && picker.tiles.size() == 3 &&
                    std::all_of(picker.tiles.begin(), picker.tiles.end(), [](auto const& tile) { return bool(tile.image.Source()); });
                assert(picker.thumbnailJobs.size() <= 4);
            });
            return loaded;
        });
        OnEditorAsync([](SettingsEditor& ui) {
            showPreview = true;
            ShowWindow(ui.child, SW_SHOW);
            ShowWindow(ui.window, SW_SHOWNOACTIVATE);
            SetForegroundWindow(ui.window);
            ui.content.UpdateLayout();
        });
        WaitFor([] {
            bool focused{};
            OnEditorAsync([&](SettingsEditor& ui) {
                focused=ui.iconPicker.search.Focus(FocusState::Programmatic);
            });
            return focused;
        });
        OnEditor([](SettingsEditor& ui) {
            auto& picker = ui.iconPicker;
            SetFocus(ui.window); // Host receives focus again after activation.
            printf("Search host-focus handoff: %s\n", GetFocus() == ui.window ? "stuck on host" : "XAML input");
            fflush(stdout);
            assert(GetFocus() != ui.window); // Keep native keyboard focus on the XAML input site.
            wchar_t focusClass[128]{};
            GetClassNameW(GetFocus(), focusClass, ARRAYSIZE(focusClass));
            assert(wcscmp(focusClass, L"Windows.UI.Input.InputSite.WindowClass") == 0);
            assert(picker.search.FocusState() != FocusState::Unfocused);
            ShowWindow(ui.window, SW_HIDE);
            showPreview = false;
            ui.content.UpdateLayout();
            auto first = picker.results.Items().GetAt(0).as<Controls::GridViewItem>();
            auto second = picker.results.Items().GetAt(1).as<Controls::GridViewItem>();
            auto a = first.TransformToVisual(picker.results).TransformPoint({0, 0});
            auto b = second.TransformToVisual(picker.results).TransformPoint({0, 0});
            assert(b.X > a.X && std::abs(a.Y - b.Y) < 1);
            assert(first.Content().try_as<Controls::Border>()); // Tiles contain images, not title rows.
            assert(winrt::unbox_value<winrt::hstring>(Controls::ToolTipService::GetToolTip(first)) ==
                L"Discord · Dashboard Icons · Original artwork");
            assert(Automation::AutomationProperties::GetName(first) == L"Discord · Dashboard Icons · Original artwork");
            assert(first.FindName(L"AdaptiveBadge").as<Controls::Border>().Visibility() == Visibility::Collapsed);
            assert(second.FindName(L"AdaptiveBadge").as<Controls::Border>().Visibility() == Visibility::Visible);
            assert(Automation::AutomationProperties::GetName(second) == L"GitHub · Dashboard Icons · Follows text color");
            assert(!picker.panel.FindName(L"Color"));
            auto delayed = std::make_shared<CatalogIconJob>();
            delayed->window = ui.window;
            delayed->generation = ++picker.generation;
            delayed->thread = CreateThread(nullptr, 0, [](void* raw) -> DWORD {
                auto& job = *static_cast<CatalogIconJob*>(raw);
                while (!job.cancel) Sleep(1);
                Sleep(600); // Simulate a request that takes time to finish cancelling.
                job.done = true;
                PostMessageW(job.window, kCatalogIconsMessage, job.generation, 0);
                return 0;
            }, delayed.get(), 0, nullptr);
            assert(delayed->thread);
            picker.thumbnailJobs.push_back(delayed);
            auto started = GetTickCount64();
            ui.iconPicker.search.Text(L"GitHub");
            auto elapsed = GetTickCount64() - started;
            printf("Search text callback: %llu ms\n", elapsed);
            fflush(stdout);
            assert(elapsed < 100); // Typing must not wait for network cancellation.
            assert(ui.iconPicker.search.Text() == L"GitHub");
        });
        WaitFor([] {
            bool filtered{};
            OnEditor([&](SettingsEditor& ui) {
                filtered = !ui.iconPicker.searchPending && ui.iconPicker.filtered.size() == 1;
                assert(ui.iconPicker.thumbnailJobs.size() + ui.iconPicker.retiredJobs.size() <= 4);
            });
            return filtered;
        });
        OnEditor([](SettingsEditor& ui) { ui.iconPicker.results.SelectedIndex(0); });
        WaitFor([] {
            bool ready{};
            OnEditor([&](SettingsEditor& ui) { ready = ui.iconPicker.use.IsEnabled(); });
            return ready;
        });
        OnEditor([](SettingsEditor& ui) {
            assert(ui.iconPicker.selectedFiles.Adaptive() &&
                   ui.iconPicker.selectedFiles.dark.ends_with(L"native-github-2.png"));
            assert(ui.iconPicker.preview.Source() == ui.iconPicker.tiles[0].image.Source());
            assert(ui.iconPicker.selectedName.Text() == L"GitHub · Dashboard Icons · Follows text color");
        });
        auto beforeSimple = ReadLocalFile(g_settingsPath);
        OnEditor([](SettingsEditor& ui) {
            assert(!ui.trayOverrides.contains(L"123"));
            assert(ui.iconPicker.preview.Source());
            ui.content.UpdateLayout();
            auto button = ui.iconPicker.use;
            assert(button);
            Automation::Peers::ButtonAutomationPeer peer(button);
            peer.GetPattern(Automation::Peers::PatternInterface::Invoke)
                .as<Automation::Provider::IInvokeProvider>().Invoke();
            PumpEditorFrames();
            assert(ui.trayOverrides.at(L"123").light.ends_with(L"native-github-1.png") &&
                   ui.trayOverrides.at(L"123").Adaptive());
            assert(!ui.iconPicker.open && ui.content.IsEnabled());
        });
        assert(ReadLocalFile(g_settingsPath) == beforeSimple); // Use icon only changes the draft.
        OnEditor([](SettingsEditor& ui) {
            CloseCatalogPicker(ui);
            InvokeSave(ui);
        });
        WaitFor([] { return g_customTrayIcons.contains(L"123"); });
        assert(LoadTrayImage(g_customTrayIcons.at(L"123").light).width == 128);
        OnEditor([](SettingsEditor& ui) {
            OpenCatalogPicker(ui, L"456");
            ui.iconPicker.search.Text(L"GitHub");
            ui.iconPicker.results.SelectedIndex(0);
            assert(!ui.iconPicker.thumbnailJobs.empty());
            Automation::Peers::ButtonAutomationPeer peer(ui.iconPicker.cancel);
            peer.GetPattern(Automation::Peers::PatternInterface::Invoke)
                .as<Automation::Provider::IInvokeProvider>().Invoke();
            assert(!ui.trayOverrides.contains(L"456") && !ui.iconPicker.job &&
                ui.iconPicker.thumbnailJobs.empty() && ui.iconPicker.retiredJobs.empty());
            assert(!ui.iconPicker.open && ui.content.IsEnabled());
            ui.trayOverrides.clear();
            InvokeSave(ui);
        });
        WaitFor([] { return g_customTrayIcons.empty(); });
        puts("PASS: Dashboard grid, Auto badges, automatic variants, search, selection, and cancellation");

        CheckTrayPixels(customFile,simpleDirectory,directory);
        puts("PASS: native grayscale pixels and transparency, live icon updates, custom replacement, and restoration");
        SendMessageW(editor, WM_COMMAND, kEditorOpenFile, 0);
        assert(editorOpens == 1);
        EditorTask pickerTask{[](SettingsEditor& ui) { ChooseTrayImage(ui, L"123"); }, {}};
        PostMessageW(editor, kTestEditorMessage, 0, reinterpret_cast<LPARAM>(&pickerTask));
        WaitFor([&] {
            bool visible = false;
            EnumThreadWindows(GetWindowThreadProcessId(editor, nullptr), [](HWND window, LPARAM data) -> BOOL {
                wchar_t name[64]{};
                GetClassNameW(window, name, ARRAYSIZE(name));
                if (IsWindowVisible(window) && wcscmp(name, L"#32770") == 0)
                    *reinterpret_cast<bool*>(data) = true;
                return TRUE;
            }, reinterpret_cast<LPARAM>(&visible));
            return visible;
        });
        CloseSettingsEditor();
        assert(!pickerTask.error);
        puts("PASS: closing settings cancels the native file picker and joins the UI thread");
        assert(!g_editorWindow && !g_editorThread);
        assert(DeleteFileW(g_settingsPath.c_str()));
        g_appliedSettingsDocument = "schemaVersion=1\n[settings]\ncornerRadius=88\n";
        OpenLocalSettings();
        WaitFor([] { return g_editorWindow.load() != nullptr; }); PinEditorToPrimary();
        assert(LocalInt(ReadLocalSettings(g_settingsPath), L"cornerRadius") == 12);
        CloseSettingsEditor(); // XAML can be closed and recreated in the same process.
        assert(!g_editorWindow && !g_editorThread);
        assert(FlyoutShowAtOptions_Hook(winrt::get_abi(extended), expectedTarget, expectedOptions) == S_FALSE);
        assert(menu.Items().Size() == 5 && calls == 2); // No duplicate.
        RestoreSettingsMenus();
        assert(menu.Items().Size() == 3 && g_settingsMenus.empty());
        invoke.Invoke();
        assert(!g_editorWindow && !g_editorThread); // Click callback is revoked.
        target.Name(L"SomeOtherButton");
        assert(FlyoutShowAt_Hook(winrt::get_abi(base), expectedTarget) == S_FALSE);
        assert(menu.Items().Size() == 3);
        puts("PASS: both ShowAt paths, target filtering, no duplicate entries, and UI-thread cleanup");
        puts("PASS: XAML controls, slider sync, pages, save/apply, invalid input, external edits, reload, and reopen/shutdown");
        assert(windhawkSettingReads == 0);
        puts("PASS: built-in defaults, TOML-only persistence, and zero Windhawk settings reads");
        SendMessageW(g_dispatch, kRestoreMessage, 0, 0);
    }
    xaml.Close();
    return 0;
}
int Preview(wchar_t* file, bool trayDemo = false, int page = -1) {
    showPreview = true;
    livePreviewData = true;
    previewTrayDemo = trayDemo;
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    g_settingsPath = file;
    SetModConfiguration(ReadLocalSettings(g_settingsPath));
    CreateTestDispatcher();
    OpenLocalSettings();
    WaitFor([] { return g_editorWindow.load() != nullptr; }); PinEditorToPrimary();
    SetWindowTextW(g_editorWindow, L"Ultrawide taskbar settings — preview copy");
    traceNativeFrame = true;
    testEditorOriginal = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(
        g_editorWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(TestEditorProc)));
    if (trayDemo) {
        OnEditor([](SettingsEditor& ui) {
            {
                std::lock_guard lock(g_trayCatalogMutex);
                testTrayCatalog = {
                    {L"123", L"Example app", SnapshotTrayBitmap(ui.preview.appIcons[0].Source()), false},
                    {L"456", L"Overflow example", SnapshotTrayBitmap(ui.preview.appIcons[1].Source()), true}};
            }
            ui.controls[2].input.as<Controls::ToggleSwitch>().IsOn(true);
            ui.navigation.SelectedIndex(2);
        });
    }
    if (page >= 0) OnEditor([page](SettingsEditor& ui) { ui.navigation.SelectedIndex(page); });
    // Client-pixel rects for check_glass_controls.py, refreshed while the window lives.
    for (auto next = GetTickCount64(); g_editorWindow;) {
        MSG message{};
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        if (page >= 0 && GetTickCount64() >= next) {
            next = GetTickCount64() + 500;
            OnEditor([](SettingsEditor& ui) {
                float scale = GetDpiForWindow(ui.window) / 96.0f;
                auto print = [&](std::wstring const& name, FrameworkElement const& element) {
                    auto origin = element.TransformToVisual(ui.content).TransformPoint({0, 0});
                    printf("GLASS-RECT %ls %ld %ld %ld %ld\n", name.c_str(), std::lround(origin.X * scale),
                           std::lround(origin.Y * scale), std::lround((origin.X + element.ActualWidth()) * scale),
                           std::lround((origin.Y + element.ActualHeight()) * scale));
                };
                for (uint32_t i = 0; i < ui.navigation.Items().Size(); ++i)
                    print(L"nav" + std::to_wstring(i), ui.navigation.Items().GetAt(i).as<FrameworkElement>());
                print(L"save", ui.save);
                print(L"sidebar", ui.sidebar);
                // Template parts exist once their control's template is applied.
                if (auto pill = TestTemplatePart(ui.controls[&SettingDefinition(L"ui.bodyBorder") - kSettingFields].input,
                                                 L"SwitchKnobBounds"))
                    print(L"pill", pill);
                if (auto thumb = TestTemplatePart(ui.controls[&SettingDefinition(L"ui.bodyBorderWidth") - kSettingFields].slider,
                                                  L"HorizontalThumb"))
                    print(L"thumb", thumb);
                if (auto combo = ui.controls[&SettingDefinition(L"ui.bodyBorderColor") - kSettingFields].input)
                    print(L"combo", combo);
                if (auto text = ui.controls[&SettingDefinition(L"ui.bodyBorderWidth") - kSettingFields].input)
                    print(L"text", text);
                if (ui.navigation) print(L"navlist", ui.navigation);
                if (ui.scroll) print(L"scroll", ui.scroll);
                if (auto thumb = TestTemplatePart(ui.scroll, L"VerticalThumb")) print(L"vthumb", thumb);
                printf("GLASS-SELECTED %d\n", ui.navigation.SelectedIndex());
                fflush(stdout);
            });
        }
        Sleep(10);
    }
    CloseSettingsEditor();
    SendMessageW(g_dispatch, kRestoreMessage, 0, 0);
    return 0;
}
// --motion-only drives the real mouse. The button comes back up and the cursor returns even when
// an assert aborts the process.
bool motionButtonDown{};
POINT motionCursor{};
void ReleaseMotionInput(int) {
    if (std::exchange(motionButtonDown, false)) {
        INPUT up{};
        up.type = INPUT_MOUSE;
        up.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &up, sizeof(up));
    }
    SetCursorPos(motionCursor.x, motionCursor.y);
}
// Opens the editor with default settings in a private directory, runs one editor-thread check, closes it.
int RunEditorCheck(wchar_t const* directoryName, void (*check)(SettingsEditor&)) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    std::filesystem::path directory(directoryName);
    std::filesystem::create_directories(directory);
    g_settingsPath = (directory / L"settings.toml").wstring();
    assert(WriteLocalFile(g_settingsPath, SettingsDocument(nullptr), true));
    SetModConfiguration(ReadLocalSettings(g_settingsPath));
    CreateTestDispatcher();
    OpenLocalSettings();
    WaitFor([] { return g_editorWindow.load() != nullptr; }); PinEditorToPrimary();
    testEditorOriginal = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(
        g_editorWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(TestEditorProc)));
    OnEditorAsync(check);
    CloseSettingsEditor();
    return 0;
}

// --resize-only: real mouse input over the topmost test editor's frame. Every edge shows its sizing cursor however the
// pointer arrives and while a resize is held, a bottom-right point on screen is HTBOTTOMRIGHT with its cursor, and a
// 400 events/s edge drag keeps up with the cursor and stops with it. Each part reports before any assert fires.
int RunResizeChecks(wchar_t const* directoryName) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    std::filesystem::path directory(directoryName);
    std::filesystem::create_directories(directory);
    g_settingsPath = (directory / L"settings.toml").wstring();
    assert(WriteLocalFile(g_settingsPath, SettingsDocument(nullptr), true));
    SetModConfiguration(ReadLocalSettings(g_settingsPath));
    CreateTestDispatcher();
    OpenLocalSettings();
    WaitFor([] { return g_editorWindow.load() != nullptr; }); PinEditorToPrimary();
    testEditorOriginal = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(
        g_editorWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(TestEditorProc)));
    GetCursorPos(&motionCursor);
    signal(SIGABRT, ReleaseMotionInput);
    struct Restore { ~Restore() { ReleaseMotionInput(0); } } restore;
    auto pumpFor = [](int ms) { auto end = GetTickCount64() + ms; WaitFor([&] { return GetTickCount64() >= end; }); };
    HWND window{};
    OnEditorAsync([&](SettingsEditor& ui) {
        window = ui.window;
        ShowWindow(ui.child, SW_SHOWNOACTIVATE);
        ShowWindow(ui.window, SW_SHOWNOACTIVATE);
        SetWindowPos(ui.window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        SetForegroundWindow(ui.window);
    });
    pumpFor(1500);
    // The glass body's renderers start after the window shows; a probe before them read the arrow once.
    for (int i = 0; i < 100; ++i) {
        bool ready{};
        OnEditorAsync([&](SettingsEditor& ui) { ready = ui.backgroundRenderer && ui.desktopRenderer; });
        if (ready) break;
        pumpFor(100);
    }
    pumpFor(500);
    RECT frame{};
    GetWindowRect(window, &frame);
    auto send = [](DWORD flags, POINT point = {}) {
        INPUT input{};
        input.type = INPUT_MOUSE;
        input.mi.dx = LONG(std::lround(double(point.x - GetSystemMetrics(SM_XVIRTUALSCREEN)) * 65535 /
                                       (GetSystemMetrics(SM_CXVIRTUALSCREEN) - 1)));
        input.mi.dy = LONG(std::lround(double(point.y - GetSystemMetrics(SM_YVIRTUALSCREEN)) * 65535 /
                                       (GetSystemMetrics(SM_CYVIRTUALSCREEN) - 1)));
        input.mi.dwFlags = flags;
        assert(SendInput(1, &input, sizeof(input)) == 1);
    };
    constexpr DWORD move = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
    auto cursorName = [] {
        CURSORINFO info{sizeof(info)};
        GetCursorInfo(&info);
        std::pair<LPCWSTR, char const*> kinds[]{{IDC_ARROW, "ARROW"}, {IDC_SIZEWE, "SIZEWE"}, {IDC_SIZENS, "SIZENS"},
            {IDC_SIZENWSE, "SIZENWSE"}, {IDC_SIZENESW, "SIZENESW"}, {IDC_IBEAM, "IBEAM"}, {IDC_HAND, "HAND"}};
        for (auto [id, name] : kinds)
            if (LoadCursorW(nullptr, id) == info.hCursor) return std::string(name);
        char text[32];
        snprintf(text, sizeof text, "%p", static_cast<void*>(info.hCursor));
        return std::string(text);
    };
    auto ours = [](POINT point) {
        DWORD pid{};
        GetWindowThreadProcessId(WindowFromPoint(point), &pid);
        return pid == GetCurrentProcessId();
    };
    std::vector<std::string> cursorFailures, cornerFailures, dragFailures;
    // Something else moving the mouse, or a window coming up over this one, makes a run say nothing about the frame.
    std::string interference;
    // A drag that fails to resize leaves its end point off the window, so drags check only the cursor.
    auto still = [&](std::string const& name, POINT expected, bool shown = true) {
        POINT now{};
        GetCursorPos(&now);
        if (interference.empty() && (now.x != expected.x || now.y != expected.y || (shown && !ours(expected))))
            interference = name + " (cursor at " + std::to_string(now.x) + "," + std::to_string(now.y) + ")";
    };
    auto hitAt = [&](POINT point) { return SendMessageW(window, WM_NCHITTEST, 0, MAKELPARAM(point.x, point.y)); };
    // Arrives at `point` in a short approach from `from`, as a hand moves, or in one jump when `steps` is 1. A null
    // `cursor` expects any cursor but a sizing one.
    auto probe = [&](std::string const& name, POINT point, POINT from, int steps, LRESULT hit, char const* cursor,
                     std::vector<std::string>& failures) {
        bool shown = ours(point);
        std::string shows = "-";
        if (shown) {
            send(move, from);
            pumpFor(300);
            for (int k = 1; k <= steps; ++k) {
                send(move, {from.x + (point.x - from.x) * k / steps, from.y + (point.y - from.y) * k / steps});
                pumpFor(16);
            }
            pumpFor(400);
            shows = cursorName();
            still(name, point);
        }
        printf("RESIZE %s at %ld,%ld shown=%d hit=%lld cursor=%s\n", name.c_str(), point.x, point.y, shown,
               static_cast<long long>(hitAt(point)), shows.c_str());
        bool cursorOk = cursor ? shows == cursor : shows != "-" && shows.rfind("SIZE", 0) != 0;
        if (!shown || hitAt(point) != hit || !cursorOk) failures.push_back(name);
    };
    LONG l = frame.left, t = frame.top, r = frame.right, b = frame.bottom, mx = (l + r) / 2, my = (t + b) / 2;
    struct Edge { char const* name; POINT point, inside, outside; LRESULT hit; char const* cursor; };
    Edge const edges[]{{"right", {r - 3, my}, {r - 60, my}, {r + 60, my}, HTRIGHT, "SIZEWE"},
                       {"left", {l + 2, my}, {l + 60, my}, {l - 60, my}, HTLEFT, "SIZEWE"},
                       {"bottom", {mx, b - 3}, {mx, b - 60}, {mx, b + 60}, HTBOTTOM, "SIZENS"},
                       {"top", {mx, t + 2}, {mx, t + 60}, {mx, t - 60}, HTTOP, "SIZENS"}};
    for (auto const& edge : edges) {
        std::string name = std::string(edge.name) + " edge";
        probe(name + " from outside", edge.point, edge.outside, 6, edge.hit, edge.cursor, cursorFailures);
        probe(name + " by one jump from the body", edge.point, {mx, my}, 1, edge.hit, edge.cursor, cursorFailures);
        probe(name + " from inside", edge.point, edge.inside, 6, edge.hit, edge.cursor, cursorFailures);
    }
    probe("body", {mx, my}, {mx - 60, my}, 6, HTCLIENT, nullptr, cursorFailures);
    // The window region rounds the corner off, so only a point beside the corner, along an edge, can be on screen.
    std::optional<POINT> corner;
    for (LONG k = 2; k < 200 && !corner; ++k)
        for (POINT point : {POINT{r - k, b - 3}, POINT{r - 3, b - k}})
            if (!corner && ours(point) && hitAt(point) == HTBOTTOMRIGHT) corner = point;
    if (corner) probe("bottom-right corner", *corner, {corner->x - 60, corner->y - 60}, 6, HTBOTTOMRIGHT, "SIZENWSE",
                      cornerFailures);
    else {
        puts("RESIZE bottom-right corner: no point is both shown and HTBOTTOMRIGHT");
        cornerFailures.push_back("no bottom-right point is both shown and HTBOTTOMRIGHT");
    }
    // Held: press on the right edge, pull 30 px out, read the cursor, release.
    POINT grip{r - 3, my};
    if (ours(grip)) {
        send(move, {r - 60, my});
        pumpFor(120);
        send(move, grip);
        pumpFor(300);
        motionButtonDown = true;
        send(MOUSEEVENTF_LEFTDOWN);
        pumpFor(150);
        for (int k = 1; k <= 30; ++k) { send(move, {grip.x + k, grip.y}); pumpFor(16); }
        pumpFor(300);
        RECT held{};
        GetWindowRect(window, &held);
        std::string shows = cursorName();
        still("held resize", {grip.x + 30, grip.y}, false);
        printf("RESIZE held resize cursor=%s width %ld -> %ld\n", shows.c_str(), r - l, held.right - held.left);
        if (shows != "SIZEWE") cursorFailures.push_back("held resize");
        motionButtonDown = false;
        send(MOUSEEVENTF_LEFTUP);
        pumpFor(300);
    } else cursorFailures.push_back("held resize: the right edge is not shown");
    // A real mouse's event rate: pull the right edge 200 px out in 0.5 s at 400 events/s, hold still, release.
    GetWindowRect(window, &frame);
    POINT fast{frame.right - 3, (frame.top + frame.bottom) / 2};
    if (ours(fast)) {
        LARGE_INTEGER frequency{}, start{}, now{};
        QueryPerformanceFrequency(&frequency);
        auto seconds = [&] { QueryPerformanceCounter(&now); return double(now.QuadPart - start.QuadPart) / frequency.QuadPart; };
        auto width = [&] { RECT rect{}; GetWindowRect(window, &rect); return rect.right - rect.left; };
        send(move, {fast.x - 60, fast.y});
        pumpFor(120);
        send(move, fast);
        pumpFor(300);
        motionButtonDown = true;
        send(MOUSEEVENTF_LEFTDOWN);
        pumpFor(150);
        LONG width0 = width(), worst = 0;
        QueryPerformanceCounter(&start);
        for (int k = 1; k <= 200; ++k) {
            send(move, {fast.x + k, fast.y});
            worst = std::max(worst, width0 + k - width());
            while (seconds() < k / 400.0) Sleep(0);
        }
        LONG trail = width0 + 200 - width();
        still("fast drag", {fast.x + 200, fast.y}, false);
        double stopped = seconds(), lastChange = stopped;
        for (LONG previous = width(); seconds() - stopped < 2;) {
            Sleep(2);
            if (LONG w = width(); w != previous) { previous = w; lastChange = seconds(); }
        }
        motionButtonDown = false;
        send(MOUSEEVENTF_LEFTUP);
        pumpFor(300);
        double kept = 1000 * (lastChange - stopped);
        LONG ended = width();
        printf("RESIZE fast drag 200 px at 400 events/s: trail at the stop %ld px (worst %ld), kept resizing %.0f ms "
               "after the cursor stopped, final width %ld of %ld\n", trail, worst, kept, ended, width0 + 200);
        // Explorer replays the queued drag (140 px and 1.1-1.35 s on 0.4.5); this process barely does (36 px and
        // 95 ms on 0.4.5, 18 px and 61 ms on 0.4.6), so trail and tail are printed, and the live fast drag judges them.
        if (std::abs(ended - (width0 + 200)) > 8)
            dragFailures.push_back("final width " + std::to_string(ended) + " is not the cursor's " + std::to_string(width0 + 200));
    } else dragFailures.push_back("fast drag: the right edge is not shown");
    CloseSettingsEditor();
    if (!interference.empty()) {
        printf("RESIZE INVALID: something else moved the mouse or covered the window at %s; no verdict\n", interference.c_str());
        return 3;
    }
    auto report = [](std::vector<std::string> const& failures, char const* pass) {
        if (failures.empty()) { printf("PASS: %s\n", pass); return; }
        for (auto const& failure : failures) printf("FAIL: %s\n", failure.c_str());
    };
    report(cursorFailures, "every edge shows its sizing cursor arriving from outside, by one jump from the body and from "
                           "inside, the body shows the arrow, and a held right-edge resize keeps SIZEWE");
    report(cornerFailures, "a bottom-right point beside the rounded corner is on screen, HTBOTTOMRIGHT and shows SIZENWSE");
    report(dragFailures, "a 200 px right-edge drag at 400 events/s ends with the window's edge at the cursor");
    assert(cursorFailures.empty() && cornerFailures.empty() && dragFailures.empty());
    return 0;
}

int RunMotionChecks(wchar_t const* directoryName) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    std::filesystem::path directory(directoryName);
    std::filesystem::create_directories(directory);
    g_settingsPath = (directory / L"settings.toml").wstring();
    assert(WriteLocalFile(g_settingsPath, SettingsDocument(nullptr), true));
    SetModConfiguration(ReadLocalSettings(g_settingsPath));
    CreateTestDispatcher();
    testClientAnimations = 1;
    OpenLocalSettings();
    WaitFor([] { return g_editorWindow.load() != nullptr; }); PinEditorToPrimary();
    testEditorOriginal = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(
        g_editorWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(TestEditorProc)));
    GetCursorPos(&motionCursor);
    signal(SIGABRT, ReleaseMotionInput);
    struct Restore { ~Restore() { ReleaseMotionInput(0); } } restore;
    auto pumpFor = [](int ms) { auto end = GetTickCount64() + ms; WaitFor([&] { return GetTickCount64() >= end; }); };
    float scale{};
    OnEditorAsync([&](SettingsEditor& ui) {
        showPreview = true;
        ShowWindow(ui.child, SW_SHOWNOACTIVATE);
        ShowWindow(ui.window, SW_SHOWNOACTIVATE);
        SetWindowPos(ui.window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        SetForegroundWindow(ui.window);
        ui.navigation.SelectedIndex(3);
        scale = GetDpiForWindow(ui.window) / 96.0f;
    });
    pumpFor(600);
    using Rect = winrt::Windows::Foundation::Rect;
    auto control = [](SettingsEditor& ui, PCWSTR key) -> EditorControl& {
        return ui.controls[&SettingDefinition(key) - kSettingFields];
    };
    // DIPs from the content origin, which is the client origin.
    auto bounds = [&](std::function<FrameworkElement(SettingsEditor&)> element) {
        Rect result{};
        OnEditorAsync([&](SettingsEditor& ui) {
            auto target = element(ui);
            assert(target && target.ActualWidth() > 0);
            auto origin = target.TransformToVisual(ui.content).TransformPoint({0, 0});
            result = {origin.X, origin.Y, float(target.ActualWidth()), float(target.ActualHeight())};
        });
        return result;
    };
    struct Lens { float x, y, halfWidth, halfHeight, alpha; };
    auto lensWhere = [&](auto match) {
        std::optional<Lens> found;
        OnEditorAsync([&](SettingsEditor& ui) {
            for (auto const& lens : EditorLenses(ui)) {
                Lens candidate{lens.native[0] / scale, lens.native[1] / scale, lens.native[2] / scale, lens.native[3] / scale, lens.alpha};
                if (!found && lens.kind == 2 && match(candidate)) found = candidate;
            }
        });
        assert(found);
        return *found;
    };
    auto lensAt = [&](float x, float y) {
        return lensWhere([&](Lens const& lens) { return std::abs(lens.x - x) < 1 && std::abs(lens.y - y) < 1; });
    };
    auto send = [](DWORD flags, POINT point = {}) {
        INPUT input{};
        input.type = INPUT_MOUSE;
        input.mi.dx = LONG(std::lround(double(point.x - GetSystemMetrics(SM_XVIRTUALSCREEN)) * 65535 /
                                       (GetSystemMetrics(SM_CXVIRTUALSCREEN) - 1)));
        input.mi.dy = LONG(std::lround(double(point.y - GetSystemMetrics(SM_YVIRTUALSCREEN)) * 65535 /
                                       (GetSystemMetrics(SM_CYVIRTUALSCREEN) - 1)));
        input.mi.dwFlags = flags;
        assert(SendInput(1, &input, sizeof(input)) == 1);
    };
    // Enters from the left, so XAML sees the pointer arrive, then holds the left button.
    auto hold = [&](float x, float y) {
        POINT point{LONG(std::lround(x * scale)), LONG(std::lround(y * scale))};
        OnEditorAsync([&](SettingsEditor& ui) {
            assert(ClientToScreen(ui.window, &point));
            DWORD pid{};
            GetWindowThreadProcessId(WindowFromPoint(point), &pid);
            assert(pid == GetCurrentProcessId());
        });
        constexpr DWORD move = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
        send(move, {point.x - 30, point.y});
        pumpFor(60);
        send(move, point);
        pumpFor(120);
        motionButtonDown = true;
        send(MOUSEEVENTF_LEFTDOWN);
    };
    auto release = [&] { motionButtonDown = false; send(MOUSEEVENTF_LEFTUP); };

    auto save = bounds([](SettingsEditor& ui) { return ui.save; });
    float saveX = save.X + save.Width / 2, saveY = save.Y + save.Height / 2;
    auto restSave = lensAt(saveX, saveY);
    hold(saveX, saveY);
    pumpFor(250);
    auto heldSave = lensAt(saveX, saveY);
    double heldScale{};
    OnEditorAsync([&](SettingsEditor& ui) {
        auto transform = ui.save.RenderTransform().try_as<Media::ScaleTransform>();
        heldScale = transform && transform.ScaleX() == transform.ScaleY() ? transform.ScaleX() : 0;
    });
    release();
    pumpFor(400);
    auto releasedSave = lensAt(saveX, saveY);
    bool scaleCleared{};
    OnEditorAsync([&](SettingsEditor& ui) {
        scaleCleared = ui.save.ReadLocalValue(UIElement::RenderTransformProperty()) == DependencyProperty::UnsetValue();
    });
    printf("Save half-size %.2fx%.2f DIP, held %.2fx%.2f (x%.3f, button ScaleTransform x%.3f), released %.2fx%.2f "
           "(transform cleared %d)\n", restSave.halfWidth, restSave.halfHeight, heldSave.halfWidth, heldSave.halfHeight,
           heldSave.halfWidth / restSave.halfWidth, heldScale, releasedSave.halfWidth, releasedSave.halfHeight, scaleCleared);
    // The button itself grows, and the lens reads its scaled bounds rather than inflating a rest-size rect.
    assert(std::abs(heldScale - 1.15) < 1e-3);
    assert(std::abs(heldSave.halfWidth - restSave.halfWidth * 1.15f) * scale < 1 &&
           std::abs(heldSave.halfHeight - restSave.halfHeight * 1.15f) * scale < 1);
    assert(std::abs(releasedSave.halfWidth - restSave.halfWidth) * scale < .5f &&
           std::abs(releasedSave.halfHeight - restSave.halfHeight) * scale < .5f && scaleCleared);

    auto thumbOf = [&](SettingsEditor& ui) -> FrameworkElement {
        return TestTemplatePart(control(ui, L"ui.bodyBorderWidth").slider, L"HorizontalThumb");
    };
    auto thumbLens = [&] { auto thumb = bounds(thumbOf); return lensAt(thumb.X + thumb.Width / 2, thumb.Y + thumb.Height / 2); };
    double width{};
    OnEditorAsync([&](SettingsEditor& ui) { width = control(ui, L"ui.bodyBorderWidth").slider.Value(); });
    auto thumb = bounds(thumbOf);
    auto restThumb = thumbLens();
    hold(thumb.X + thumb.Width / 2, thumb.Y + thumb.Height / 2);
    pumpFor(250);
    auto heldThumb = thumbLens(); // Found at the thumb's current place, in case the press moved it.
    release();
    pumpFor(400);
    auto releasedThumb = thumbLens();
    OnEditorAsync([&](SettingsEditor& ui) { control(ui, L"ui.bodyBorderWidth").slider.Value(width); });
    printf("Slider thumb half-size %.2fx%.2f DIP alpha %.2f, held %.2fx%.2f (height x%.3f, width/height %.3f) alpha %.2f, "
           "released %.2fx%.2f alpha %.2f\n", restThumb.halfWidth, restThumb.halfHeight, restThumb.alpha,
           heldThumb.halfWidth, heldThumb.halfHeight, heldThumb.halfHeight / restThumb.halfHeight,
           heldThumb.halfWidth / heldThumb.halfHeight, heldThumb.alpha, releasedThumb.halfWidth, releasedThumb.halfHeight,
           releasedThumb.alpha);
    assert(restThumb.alpha > .99f && releasedThumb.alpha > .99f && heldThumb.alpha < .15f);
    assert(std::abs(heldThumb.halfHeight - restThumb.halfHeight * 1.667f) * scale < 1 &&
           std::abs(heldThumb.halfWidth - heldThumb.halfHeight * 1.5f) * scale < 1);
    assert(std::abs(releasedThumb.halfWidth - restThumb.halfWidth) * scale < .5f &&
           std::abs(releasedThumb.halfHeight - restThumb.halfHeight) * scale < .5f);

    // A press that starts on Save and is released over the caption area belongs to XAML to the end.
    // The frame router used to take those messages, so Save never saw its release and XAML ate the next press.
    hold(saveX, saveY);
    pumpFor(400);
    { POINT header{60, 12}; OnEditorAsync([&](SettingsEditor& ui) { ClientToScreen(ui.window, &header); });
      send(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK, header); }
    pumpFor(200);
    release();
    pumpFor(500);
    auto saveAfterDrag = lensAt(saveX, saveY);
    thumb = bounds(thumbOf);
    hold(thumb.X + thumb.Width / 2, thumb.Y + thumb.Height / 2);
    pumpFor(250);
    auto thumbAfterDrag = thumbLens();
    size_t presses{};
    bool sliderPressed{};
    OnEditorAsync([&](SettingsEditor& ui) {
        presses = ui.presses.size();
        sliderPressed = presses == 1 && ui.presses[0].element.try_as<Controls::Slider>() && ui.presses[0].motion.target == 1;
    });
    release();
    pumpFor(400);
    OnEditorAsync([&](SettingsEditor& ui) { control(ui, L"ui.bodyBorderWidth").slider.Value(width); });
    printf("Save released over the caption: Save half-size %.2fx%.2f, next thumb hold alpha %.2f with %zu press(es)\n",
           saveAfterDrag.halfWidth, saveAfterDrag.halfHeight, thumbAfterDrag.alpha, presses);
    assert(std::abs(saveAfterDrag.halfWidth - restSave.halfWidth) * scale < .5f);
    assert(sliderPressed && thumbAfterDrag.alpha < .15f);

    auto toggleOf = [&](SettingsEditor& ui) { return control(ui, L"ui.bodyBorder").input.as<Controls::ToggleSwitch>(); };
    bool wasOn{};
    OnEditorAsync([&](SettingsEditor& ui) { wasOn = toggleOf(ui).IsOn(); });
    auto pill = bounds([&](SettingsEditor& ui) { return TestTemplatePart(toggleOf(ui), L"SwitchKnobBounds"); });
    float size = pill.Height * .6f, inset = (pill.Height - size) / 2, pillY = pill.Y + pill.Height / 2;
    float offX = pill.X + inset + size / 2, onX = pill.X + pill.Width - inset - size / 2;
    float restX = wasOn ? onX : offX, flipX = wasOn ? offX : onX;
    // A held knob outgrows its track, so it is told apart by order: it precedes the track lens.
    auto knobLens = [&] {
        return lensWhere([&](Lens const& lens) {
            return std::abs(lens.y - pillY) < 1 && std::abs(lens.x - (pill.X + pill.Width / 2)) < pill.Width / 2;
        });
    };
    auto pillLens = [&] { return lensAt(pill.X + pill.Width / 2, pillY); };
    auto restKnob = knobLens();
    assert(std::abs(restKnob.x - restX) * scale < 1 && restKnob.alpha > .99f);
    hold(restX, pillY);
    pumpFor(250);
    auto heldKnob = knobLens();
    release();
    int between{};
    for (auto deadline = GetTickCount64() + 2000;;) {
        auto knob = knobLens();
        if (std::abs(knob.x - flipX) * scale < .5f) break;
        between += (knob.x - offX) * (knob.x - onX) < 0;
        assert(GetTickCount64() < deadline);
        pumpFor(8);
    }
    pumpFor(400);
    auto flippedKnob = knobLens(), flippedPill = pillLens();
    bool flipped{};
    OnEditorAsync([&](SettingsEditor& ui) { flipped = toggleOf(ui).IsOn() != wasOn; });
    printf("Toggle knob alpha %.2f, held alpha %.2f half-size %.2fx%.2f DIP (height x%.3f of track %.2f, width/height %.3f, "
           "centre %.2f of %.2f); %d samples mid-slide; rests at %.2f (end %.2f) alpha %.2f, track alpha %.2f\n",
           restKnob.alpha, heldKnob.alpha, heldKnob.halfWidth, heldKnob.halfHeight, heldKnob.halfHeight * 2 / pill.Height,
           pill.Height, heldKnob.halfWidth / heldKnob.halfHeight, heldKnob.x, restX, between, flippedKnob.x, flipX,
           flippedKnob.alpha, flippedPill.alpha);
    assert(heldKnob.alpha < .15f && std::abs(heldKnob.x - restX) * scale < 1 &&
           std::abs(heldKnob.halfHeight - pill.Height * 1.25f / 2) * scale < 1 &&
           std::abs(heldKnob.halfWidth - heldKnob.halfHeight * 1.6f) * scale < 1);
    assert(flipped && between > 0);
    assert(std::abs(flippedKnob.x - flipX) * scale < .5f && flippedKnob.alpha > .99f);
    assert(std::abs(flippedPill.alpha - (wasOn ? 40 : 140) / 255.0f) < .01f);
    OnEditorAsync([&](SettingsEditor& ui) { toggleOf(ui).IsOn(wasOn); });
    WaitFor([&] { return std::abs(knobLens().x - restX) * scale < .5f; }, 2000);
    pumpFor(100);
    assert(std::abs(pillLens().alpha - (wasOn ? 140 : 40) / 255.0f) < .01f);

    // A click opens the choice list inside the window, where its card can sit on a glass plate drawn above everything.
    auto comboOf = [&](SettingsEditor& ui) { return control(ui, L"ui.bodyBorderColor").input.as<Controls::ComboBox>(); };
    auto combo = bounds(comboOf);
    hold(combo.X + combo.Width / 2, combo.Y + combo.Height / 2);
    pumpFor(120);
    release();
    pumpFor(700);
    struct { bool open, constrained, material, layerFirst; Rect card; Lens first; float kind, radius; } drop{};
    auto onCard = [&](EditorLens const& lens) {
        return std::abs(lens.native[0] / scale - (drop.card.X + drop.card.Width / 2)) < 1 &&
               std::abs(lens.native[1] / scale - (drop.card.Y + drop.card.Height / 2)) < 1 &&
               std::abs(lens.native[3] / scale - drop.card.Height / 2) < 1;
    };
    auto cardLenses = [&](SettingsEditor& ui) {
        auto lenses = EditorLenses(ui);
        return int(std::count_if(lenses.begin(), lenses.end(), onCard));
    };
    OnEditorAsync([&](SettingsEditor& ui) {
        auto popup = TestTemplatePart(comboOf(ui), L"Popup").as<Controls::Primitives::Popup>();
        auto card = TestTemplatePart(popup.Child(), L"PopupBorder").as<Controls::Border>();
        drop.open = comboOf(ui).IsDropDownOpen();
        drop.constrained = popup.IsConstrainedToRootBounds();
        drop.material = card.Background() == ui.bodyMaterial.Background();
        drop.card = card.TransformToVisual(ui.content).TransformBounds({0, 0, float(card.ActualWidth()), float(card.ActualHeight())});
        // Also first among the scrolled plates, which draw above the window output.
        auto lenses = EditorLenses(ui);
        auto layerFirst = std::find_if(lenses.begin(), lenses.end(), [](EditorLens const& lens) { return lens.layer; });
        drop.layerFirst = layerFirst != lenses.end() && onCard(*layerFirst) && layerFirst->kind == 2;
        std::lock_guard lock(ui.backgroundRenderer->mutex);
        auto const& lens = ui.backgroundRenderer->lens;
        auto const* native = lens.lensNativeRects[0];
        drop.first = {native[0] / scale, native[1] / scale, native[2] / scale, native[3] / scale, lens.lensTints[0][3]};
        drop.kind = lens.lensShapeParams[0][3];
        drop.radius = lens.lensRadii[0][0] / scale;
    });
    printf("Open choice list: constrained %d, body material %d, card %.1f,%.1f %.1fx%.1f DIP; first lens kind %.0f at "
           "%.1f,%.1f half %.1fx%.1f radius %.2f alpha %.3f; first content-layer lens on the card %d\n", drop.constrained,
           drop.material, drop.card.X, drop.card.Y, drop.card.Width, drop.card.Height, drop.kind, drop.first.x, drop.first.y,
           drop.first.halfWidth, drop.first.halfHeight, drop.radius, drop.first.alpha, drop.layerFirst);
    assert(drop.open && drop.constrained && drop.material && drop.card.Height > combo.Height && drop.layerFirst);
    assert(drop.kind == 2 && std::abs(drop.first.x - (drop.card.X + drop.card.Width / 2)) * scale < 1 &&
           std::abs(drop.first.y - (drop.card.Y + drop.card.Height / 2)) * scale < 1 &&
           std::abs(drop.first.halfWidth - drop.card.Width / 2) * scale < 1 &&
           std::abs(drop.first.halfHeight - drop.card.Height / 2) * scale < 1 &&
           std::abs(drop.radius - 8) < .01f && std::abs(drop.first.alpha - 40 / 255.0f) < .01f);
    OnEditorAsync([&](SettingsEditor& ui) { comboOf(ui).IsDropDownOpen(false); });
    pumpFor(400);
    int afterClose{};
    OnEditorAsync([&](SettingsEditor& ui) { afterClose = cardLenses(ui); });
    printf("Closed choice list: %d lens(es) left on its card\n", afterClose);
    assert(afterClose == 0);
    puts("PASS: Settings UI choice list opens on a glass plate drawn first and loses it on close");

    // The lens budget on the busiest page: Liquid glass, custom, every section open, at each scroll position.
    int theme{}, preset{};
    OnEditorAsync([&](SettingsEditor& ui) {
        theme = ui.controls[0].input.as<Controls::ComboBox>().SelectedIndex();
        preset = ui.controls[9].input.as<Controls::ComboBox>().SelectedIndex();
        ui.controls[0].input.as<Controls::ComboBox>().SelectedIndex(3);
        ui.controls[9].input.as<Controls::ComboBox>().SelectedIndex(0);
        ui.navigation.SelectedIndex(1);
    });
    pumpFor(600);
    OnEditorAsync([&](SettingsEditor& ui) { for (auto const& section : ui.sections) if (section.page == 1) section.header.IsChecked(true); });
    pumpFor(600);
    double extent{};
    float client[2]{};
    OnEditorAsync([&](SettingsEditor& ui) {
        extent = ui.scroll.ScrollableHeight();
        client[0] = float(ui.content.ActualWidth()); client[1] = float(ui.content.ActualHeight());
    });
    uint32_t peak{}, layerPeak{}, positions{}, texture[2]{};
    double contentHeight{};
    for (double offset = 0;; offset = std::min(offset + 40, extent)) {
        OnEditorAsync([&](SettingsEditor& ui) { ui.scroll.ChangeView(nullptr, offset, nullptr, true); });
        pumpFor(80);
        OnEditorAsync([&](SettingsEditor& ui) {
            contentHeight = ui.contentLayer.ActualHeight();
            std::lock_guard lock(ui.backgroundRenderer->mutex);
            peak = std::max(peak, ui.backgroundRenderer->lens.lensCount);
            layerPeak = std::max(layerPeak, ui.backgroundRenderer->layerLens.lensCount);
            texture[0] = ui.backgroundRenderer->layerWidth;
            texture[1] = ui.backgroundRenderer->layerHeight;
        });
        ++positions;
        if (offset >= extent) break;
    }
    OnEditorAsync([&](SettingsEditor& ui) {
        for (auto const& section : ui.sections) if (section.page == 1) section.header.IsChecked(false);
        ui.controls[0].input.as<Controls::ComboBox>().SelectedIndex(theme);
        ui.controls[9].input.as<Controls::ComboBox>().SelectedIndex(preset);
        ui.navigation.SelectedIndex(3);
    });
    printf("Lens budget: Liquid glass page, every section open, %.0fx%.0f DIP window at scale %.2f, %u scroll positions "
           "over %.0f DIP: window output peak %u lenses (%u controls of the %zu cap, 2 panels), content layer peak %u of "
           "%zu; content %.0f DIP tall, layer %ux%u px, %.1f MB in its two buffers\n", client[0], client[1], scale,
           positions, extent, peak, peak - 2, kMaxButtonLenses - 2, layerPeak, kMaxButtonLenses, contentHeight,
           texture[0], texture[1], texture[0] * texture[1] * 8 / 1048576.0);
    // At the cap, later controls would silently lose their glass.
    assert(peak - 2 < kMaxButtonLenses - 2 && layerPeak < kMaxButtonLenses);

    OnEditorAsync([&](SettingsEditor& ui) {
        SetWindowPos(ui.window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        showPreview = false;
    });
    CloseSettingsEditor();
    testClientAnimations = -1;
    puts("PASS: Settings UI glass motion under real input");
    return 0;
}
#include "OPTICS_CHECK_PATH"
#include "THEME_CHECK_PATH"
#include "FRAME_CHECK_PATH"
#include "ADAPTIVE_CHECK_PATH"
#include "MATERIALS_CHECK_PATH"
#include "RENDERER_LIFECYCLE_CHECK_PATH"
#include "PREVIEW_PARITY_CHECK_PATH"
#include "JUMP_LIST_CHECK_PATH"
#include "LIQUID_CAPTURE_CHECK_PATH"
#include "SHAPE_PATHS_PATH"
int wmain(int argc, wchar_t** argv) {
    try {
        if (argc == 5 && wcscmp(argv[2], L"--shape-paths-only") == 0) return RunShapePaths(argv[3], argv[4]);
        if (argc == 3 && wcscmp(argv[2], L"--outline-parity-only") == 0) {
            winrt::init_apartment(winrt::apartment_type::single_threaded);
            RunOutlineParity();
            return 0;
        }
        if (argc == 3 && wcscmp(argv[2], L"--separate-morph-only") == 0) return RunEditorCheck(argv[1], CheckSeparateMorph);
        if (argc == 3 && wcscmp(argv[2], L"--dock-lead-only") == 0) return RunEditorCheck(argv[1], CheckDockLead);
        if (argc == 3 && wcscmp(argv[2], L"--tray-band-only") == 0) return RunEditorCheck(argv[1], CheckTrayBand);
        if (argc == 3 && wcscmp(argv[2], L"--resize-frame-only") == 0) return RunEditorCheck(argv[1], CheckResizeFrame);
        if (argc == 3 && wcscmp(argv[2], L"--separate-preview-only") == 0) return RunEditorCheck(argv[1], CheckSeparatePreview);
        if (argc == 3 && wcscmp(argv[2], L"--liquid-capture-only") == 0) return RunLiquidCaptureChecks(argv[1]);
        if (argc == 3 && wcscmp(argv[2], L"--jump-list-only") == 0) return RunJumpListChecks(argv[1]);
        if (argc >= 3 && wcscmp(argv[2],L"--preview-parity-only")==0)
            return RunPreviewParityChecks(argv[1],argc>=4,argc>=4 && wcscmp(argv[3],L"limits")==0);
        if (argc >= 3 && wcscmp(argv[2], L"--renderer-lifecycle-only") == 0)
            return RunRendererLifecycleChecks(argv[1],argc>=4?argv[3]:L"");
#ifdef UW_LAYOUT_REFRESH_CHECK
        if (argc == 3 && wcscmp(argv[2], L"--layout-refresh-only") == 0) return RunButtonChecks(argv[1],L"layout-refresh");
#endif
        if (argc == 3 && wcscmp(argv[2], L"--local-refresh-only") == 0) return RunButtonChecks(argv[1],L"local-refresh");
        if (argc == 3 && wcscmp(argv[2], L"--attention-motion-only") == 0) return RunButtonChecks(argv[1],L"attention-motion");
        if (argc == 3 && wcscmp(argv[2], L"--layout-precision-only") == 0) return RunButtonChecks(argv[1],L"layout-precision");
        if (argc == 3 && wcscmp(argv[2], L"--layout-cycle-only") == 0) return RunButtonChecks(argv[1],L"layout-cycle");
        if (argc == 3 && wcscmp(argv[2], L"--renderer-fallback-only") == 0) return RunButtonChecks(argv[1],L"renderer-fallback");
        if (argc == 3 && wcscmp(argv[2], L"--renderer-stop-only") == 0) return RunButtonChecks(argv[1],L"renderer-stop");
        if (argc == 3 && wcscmp(argv[2], L"--appearance-only") == 0) return RunButtonChecks(argv[1],L"appearance");
        if (argc == 3 && wcscmp(argv[2], L"--attention-hover-only") == 0) return RunButtonChecks(argv[1],L"attention-hover");
        if (argc == 3 && wcscmp(argv[2], L"--theme-fill-only") == 0) return RunButtonChecks(argv[1],L"theme-fill");
        if (argc == 3 && wcscmp(argv[2], L"--template-native") == 0) return RunButtonChecks(argv[1],L"template-native");
        if (argc == 3 && wcscmp(argv[2], L"--native-hover-baseline") == 0) return RunButtonChecks(argv[1],L"native-hover-baseline");
        if (argc == 3 && wcscmp(argv[2], L"--template-only") == 0) return RunButtonChecks(argv[1],L"template");
        if (argc == 3 && wcscmp(argv[2], L"--opacity-only") == 0) return RunButtonChecks(argv[1],L"opacity");
        if (argc == 3 && wcscmp(argv[2], L"--cleanup-only") == 0) return RunButtonChecks(argv[1],L"cleanup");
        if (argc == 3 && wcscmp(argv[2], L"--ownership-only") == 0) return RunButtonChecks(argv[1],L"ownership");
        if (argc == 3 && wcscmp(argv[2], L"--buttons-only") == 0) return RunButtonChecks(argv[1]);
        if (argc == 3 && wcscmp(argv[2], L"--materials-only") == 0) return RunMaterialsChecks(argv[1]);
        if (argc == 3 && wcscmp(argv[2], L"--motion-only") == 0) return RunMotionChecks(argv[1]);
        if (argc == 3 && wcscmp(argv[2], L"--resize-only") == 0) return RunResizeChecks(argv[1]);
        if (argc == 3 && wcscmp(argv[2], L"--adaptive-only") == 0) return RunAdaptiveChecks(argv[1]);
        if (argc == 3 && wcscmp(argv[2], L"--frame-only") == 0) return RunFrameChecks(argv[1]);
        if (argc == 3 && wcscmp(argv[2], L"--theme-only") == 0) return RunThemeChecks(argv[1]);
        if (argc == 3 && wcscmp(argv[2], L"--optics-only") == 0) return RunOpticsChecks(argv[1]);
        if (argc == 3 && wcscmp(argv[2], L"--catalog-live") == 0) {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            CatalogIconJob catalog;
            catalog.directory = argv[1];
            RunSimpleJob(catalog);
            assert(catalog.entries.size() > 9000);
            for (auto source : {L"native", L"selfhst", L"lobehub", L"simpleicons"}) {
                auto entry = std::find_if(catalog.entries.begin(), catalog.entries.end(), [&](auto const& icon) {
                    return icon.source == source && icon.slug == (wcscmp(source,L"simpleicons") == 0 ? L"dotenv" : L"github");
                });
                assert(entry != catalog.entries.end());
                CatalogIconJob icon;
                icon.directory = argv[1]; icon.entry = *entry;
                RunSimpleJob(icon);
                assert(icon.pixels.width > 0 && std::filesystem::exists(icon.files.light) &&
                    std::filesystem::exists(icon.files.dark) && icon.files.Adaptive());
                auto light = LoadTrayImage(icon.files.light), dark = LoadTrayImage(icon.files.dark);
                assert(light.pixels != dark.pixels);
                CatalogIconJob original;
                original.directory = argv[1]; original.entry = *entry;
                auto path = LoadCatalogVariant(original, 0, original.pixels);
                assert(std::filesystem::exists(path) && original.pixels.width > 0);
                if (entry->source == L"simpleicons") {
                    assert(std::any_of(original.pixels.pixels.begin(), original.pixels.pixels.end(), [](uint32_t p) {
                        return (p >> 24) > 240 && (p & 255) != ((p >> 16) & 255);
                    })); // Original retains the .ENV brand color.
                }
                printf("PASS: live source %ls light/dark artwork downloaded and cached\n", source);
            }
            printf("PASS: live Dashboard Icons catalog (%zu entries), all four sources\n", catalog.entries.size());
            return 0;
        }
        if (argc >= 3 && wcscmp(argv[2], L"--preview") == 0) return Preview(argv[1], false, argc >= 4 ? _wtoi(argv[3]) : -1);
        if (argc >= 3 && wcscmp(argv[2], L"--tray-preview") == 0) return Preview(argv[1], true, argc >= 4 ? _wtoi(argv[3]) : -1);
        return RunChecks(argc, argv);
    }
    catch (winrt::hresult_error const& error) {
        fwprintf(stderr, L"Native check failed: 0x%08X %s\n",
                 static_cast<unsigned>(error.code().value), error.message().c_str());
        return 1;
    }
}
'''.replace("SOURCE_PATH", source.as_posix()).replace(
    "OPTICS_CHECK_PATH", (Path(__file__).resolve().parent.parent / "optics-check/optics.check.inc").as_posix()).replace(
    "THEME_CHECK_PATH", (Path(__file__).resolve().parent.parent / "settings-theme/theme.check.inc").as_posix()).replace(
    "FRAME_CHECK_PATH", (Path(__file__).resolve().parent.parent / "custom-frame/frame.check.inc").as_posix()).replace(
    "ADAPTIVE_CHECK_PATH", (Path(__file__).resolve().parent.parent / "adaptive-color/adaptive.check.inc").as_posix()).replace(
    "MATERIALS_CHECK_PATH", (Path(__file__).resolve().parent.parent / "optics-check/materials.check.inc").as_posix()).replace(
    "RENDERER_LIFECYCLE_CHECK_PATH", (Path(__file__).resolve().parent.parent / "optics-check/renderer-lifecycle.check.inc").as_posix()).replace(
    "PREVIEW_PARITY_CHECK_PATH", (Path(__file__).resolve().parent.parent / "optics-check/preview-parity.check.inc").as_posix()).replace(
    "JUMP_LIST_CHECK_PATH", (Path(__file__).resolve().parent.parent / "jump-list/jump-list.check.inc").as_posix()).replace(
    "LIQUID_CAPTURE_CHECK_PATH", (Path(__file__).resolve().parent.parent / "liquid-capture/liquid-capture.check.inc").as_posix()).replace(
    "SHAPE_PATHS_PATH", (Path(__file__).resolve().parent.parent / "shape-preview/shape-paths.check.inc").as_posix())
(folder / "settings.check.cpp").write_text(harness, encoding="utf-8")
subprocess.run([*compiler, *flags, "-static", "-O0" if "--fast" in sys.argv else "-O1", "-municode",
                str(folder / "settings.check.cpp"), *libraries,
                "-Wl,-Map=" + str(folder / "settings.check.map"),  # Names a hung frame: the exe carries no symbols.
                "-o", str(folder / "settings.check.exe")], check=True)
manifest = folder / "settings.check.manifest"
manifest.write_text('''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
  <compatibility xmlns="urn:schemas-microsoft-com:compatibility.v1">
    <application>
      <maxversiontested Id="10.0.26100.0"/>
      <supportedOS Id="{8e0f7a12-bfb3-4fe8-b9a5-48fd50a15a9a}"/>
    </application>
  </compatibility>
</assembly>
''', encoding="utf-8")
subprocess.run(["rtk", "proxy",
    "C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64/mt.exe",
    "-nologo", "-manifest", str(manifest),
    "-outputresource:" + str(folder / "settings.check.exe") + ";#1"], check=True)
# --jump-list-only loads copies of this under mod-like names.
(folder / "jump-guard.c").write_text("int jump_guard;\n", encoding="utf-8")
subprocess.run([*compiler, "-target", "x86_64-w64-mingw32", "-x", "c", "-shared", "-static", str(folder / "jump-guard.c"),
                "-o", str(folder / "jump-guard.dll")], check=True)
if "--compile-only" in sys.argv:
    sys.exit(0)
if "--outline-parity-only" in sys.argv:  # No window: the trace raster against the shader field.
    RunCheck(["rtk", "proxy", str(folder / "settings.check.exe"), "-", "--outline-parity-only"], check=True)
    sys.exit(0)
for only in ("--separate-morph-only", "--dock-lead-only", "--separate-preview-only", "--tray-band-only",
             "--resize-frame-only"):  # One editor-thread check in a private editor.
    if only in sys.argv:
        with tempfile.TemporaryDirectory(prefix="windhawk-separate-check-") as temporary:
            RunCheck(["rtk", "proxy", str(folder / "settings.check.exe"), temporary, only], check=True)
        sys.exit(0)
if "--liquid-capture-only" in sys.argv:
    output = Path(__file__).resolve().parent.parent / "liquid-capture/output"
    output.mkdir(exist_ok=True)
    RunCheck(["rtk", "proxy", str(folder / "settings.check.exe"), str(output), "--liquid-capture-only"], check=True)
    sys.exit(0)
if "--jump-list-only" in sys.argv:
    output = Path(__file__).resolve().parent.parent / "jump-list/output"
    output.mkdir(exist_ok=True)
    RunCheck(["rtk", "proxy", str(folder / "settings.check.exe"), str(output), "--jump-list-only"], check=True)
    sys.exit(0)
if "--adaptive-only" in sys.argv:
    output = Path(__file__).resolve().parent.parent / "adaptive-color/output"
    output.mkdir(exist_ok=True)
    RunCheck(["rtk", "proxy", str(folder / "settings.check.exe"), str(output), "--adaptive-only"], check=True)
    assert tomllib.loads((output / "settings.toml").read_text())["settings"]["adaptive"]["windowSeconds"] == 3.5
    print("PASS: adaptive window accepted by Python's standard TOML parser")
    sys.exit(0)
if "--optics-only" in sys.argv:
    output = Path(__file__).resolve().parent.parent / "optics-check/output"
    output.mkdir(exist_ok=True)
    RunCheck(["rtk", "proxy", str(folder / "settings.check.exe"), str(output), "--optics-only"], check=True)
    saved = tomllib.loads((output / "settings.toml").read_text())
    assert saved["settings"]["liquid"]["model"] == "snell"
    print("PASS: optics settings accepted by the standard TOML parser")
    sys.exit(0)
if "--frame-only" in sys.argv:
    output = Path(__file__).resolve().parent.parent / "custom-frame/output"
    output.mkdir(exist_ok=True)
    RunCheck(["rtk", "proxy", str(folder / "settings.check.exe"), str(output), "--frame-only"], check=True)
    sys.exit(0)
if "--theme-only" in sys.argv:
    output = Path(__file__).resolve().parent.parent / "settings-theme/output"
    output.mkdir(exist_ok=True)
    RunCheck(["rtk", "proxy", str(folder / "settings.check.exe"), str(output), "--theme-only"], check=True)
    sys.exit(0)
if "--resize-only" in sys.argv:  # Real mouse input over the editor window's frame.
    with tempfile.TemporaryDirectory(prefix="windhawk-resize-check-") as temporary:
        RunCheck(["rtk", "proxy", str(folder / "settings.check.exe"), temporary, "--resize-only"], check=True)
    sys.exit(0)
if "--motion-only" in sys.argv:  # Real mouse input over the editor window.
    with tempfile.TemporaryDirectory(prefix="windhawk-motion-check-") as temporary:
        RunCheck(["rtk", "proxy", str(folder / "settings.check.exe"), temporary, "--motion-only"], check=True)
    sys.exit(0)
with tempfile.TemporaryDirectory(prefix="windhawk-settings-check-") as temporary:
    target = Path(temporary).resolve()
    assert target.parent == Path(tempfile.gettempdir()).resolve()
    assert target.name.startswith("windhawk-settings-check-")
    custom = Image.new("RGBA", (32, 32), (0, 0, 255, 255))
    custom.save(target / "custom.png")
    RunCheck(["rtk", "proxy", str(folder / "settings.check.exe"), str(target)], check=True)
    RunCheck(["rtk", "proxy", str(folder / "settings.check.exe"), str(target / "jump-list"),
                    "--jump-list-only"], check=True)
    RunCheck(["rtk", "proxy", str(folder / "settings.check.exe"), str(target / "liquid-capture"),
                    "--liquid-capture-only"], check=True)
    generated = tomllib.loads((target / "generated.toml").read_text())
    pairs = tomllib.loads((target / "icon-pairs.toml").read_text())["settings"]["tray"]["icons"]
    assert pairs["456"] == [r"C:\Icons\white_2.png", r"C:\Icons\black_2.png"]
    assert generated["schemaVersion"] == 1
    assert generated["settings"]["cornerRadius"] == 34
    assert generated["settings"]["button"]["border"] is False
    defaults = tomllib.loads((target / "defaults.toml").read_text())
    added_optics = {
        "rimRefraction": 50, "rimWidth": 3, "bevelSharpness": 10, "cornerBoost": 0,
        "rippleStrength": 0, "rippleWavelength": 12, "edgeWidth": 0, "edgeSoftness": 100,
        "frostEdge": 15, "dispersion": "independent", "dye": "#FFFFFF", "dyeStrength": 0,
        "fluidStrength": 0, "fluidRadius": 35, "fluidViscosity": 65, "glintMode": "off",
        "glintStrength": 15, "glintSpeed": 50, "glintSize": 35, "boundaryFade": 2,
    }
    assert defaults == {
        "schemaVersion": 1,
        "settings": {
            "theme": "native", "fontColor": "default",
            "adaptive": {"windowSeconds": 2},
            "ui": {"panelBlur": 25, "panelRefraction": 100,
                   "panelDepth": 40, "panelHighlights": 5,
                   "glassModel": "biconvex", "glassSurface": "circle",
                   "glassBezelWidth": 12, "glassThickness": 20, "glassRefractiveIndex": 1.5,
                   "glassMagnification": 0, "glassDistortion": 0, "glassChromaticAberration": 0,
                   "glassBrightness": 0, "glassSaturation": 0, "glassFresnel": 100,
                   "glassLighting": "rich", "glassLightAngle": -60,
                   "glassRimOpacity": 40, "glassRimSaturation": 400, "panelOpacity": 100,
                   "glassEdgeHighlight": 0,
                   "bodyBorder": True, "bodyBorderColor": "grey",
                   "bodyBorderCustomColor": "#FFFFFF", "bodyBorderWidth": 1, "bodyBorderOpacity": 100,
                   **{"glass" + key[0].upper() + key[1:]: value for key, value in added_optics.items()}},
            "dockBorder": False, "dockBorderColor": "grey",
            "dockBorderCustomColor": "#FFFFFF", "dockBorderWidth": 1, "dockBorderOpacity": 100,
            "cornerRadius": 12, "buttonCornerRadius": 12,
            "buttonVerticalPadding": 4, "edgeDistance": 4,
            "shape": "island", "alignment": "group", "layout": "unified", "innerArcs": False,
            "smallerTray": False, "trayReduction": 8, "slantedEnds": False, "slantAngle": 70, "barDistance": 12,
            "concaveRadius": 18,
            "button": {"border": True, "liquidLayer": False},
            "tray": {"grayscale": False, "enabled": True,
                     "highlightPadding": 8, "highlightRadius": 12},
            "liquid": {
                "preset": "custom", "zRadius": 40, "refraction": 69,
                "chromaticAberration": 5, "edgeHighlight": 5, "specular": 0,
                "fresnel": 100, "opacity": 100, "brightness": 0, "blur": 0,
                "model": "biconvex", "surface": "circle", "bezelWidth": 12,
                "thickness": 20, "refractiveIndex": 1.5, "magnification": 0,
                "distortion": 0, "saturation": 0, "tint": 0, "lighting": "original",
                "lightAngle": -60, "rimOpacity": 40, "rimSaturation": 400,
                "shadowOpacity": 0, "shadowBlur": 12, "shadowOffset": 4,
                **added_optics,
            },
        },
    }
    print("PASS: generated settings accepted by Python's standard TOML parser")
    if "--live-catalog" in sys.argv:
        live = target / "live-catalog"
        RunCheck(["rtk", "proxy", str(folder / "settings.check.exe"), str(live), "--catalog-live"], check=True)
