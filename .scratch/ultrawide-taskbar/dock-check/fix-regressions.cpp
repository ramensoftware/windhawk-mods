#include <windows.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>
#ifndef UW_REVIEW_SOURCE
#define UW_REVIEW_SOURCE "../../../mods/windows-11-better-ultrawide-taskbar.wh.cpp"
#endif
#define UWPANEL_PENDING_PROPERTY L"Windhawk.UWPOS09.PanelPending.FixCheck"
#include <cstdio>
#include UW_REVIEW_SOURCE
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <cstdio>
#include <stdexcept>

namespace regression {
int failures{};
void Check(bool condition, const char* name) {
    printf("%s: %s\n", condition ? "PASS" : "FAIL", name);
    if (!condition) ++failures;
}
void Pump(unsigned milliseconds) {
    ULONGLONG until = GetTickCount64() + milliseconds;
    do {
        MSG message{};
        while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        Sleep(1);
    } while (GetTickCount64() < until);
}
template<class Predicate>
bool Until(Predicate predicate, unsigned timeout = 2500) {
    ULONGLONG end = GetTickCount64() + timeout;
    do {
        if (predicate()) return true;
        Pump(5);
    } while (GetTickCount64() < end);
    return predicate();
}
LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    if (message == WM_PAINT) {
        PAINTSTRUCT paint{};
        auto dc = BeginPaint(window, &paint);
        RECT rect{};
        GetClientRect(window, &rect);
        HBRUSH brush = CreateSolidBrush(static_cast<COLORREF>(GetWindowLongPtr(window, GWLP_USERDATA)));
        FillRect(dc, &rect, brush);
        DeleteObject(brush);
        EndPaint(window, &paint);
        return 0;
    }
    return DefWindowProc(window, message, wp, lp);
}
struct Window {
    HWND handle{};
    explicit Window(const wchar_t* title, int width, int height, int x = 80, int y = 80) {
        static bool registered = [] {
            WNDCLASS wc{};
            wc.lpfnWndProc = WindowProc;
            wc.hInstance = GetModuleHandle(nullptr);
            wc.lpszClassName = L"Windhawk.Ultrawide.FixFixture";
            if (!RegisterClass(&wc)) throw std::runtime_error("fixture class");
            return true;
        }();
        (void)registered;
        handle = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            L"Windhawk.Ultrawide.FixFixture", title, WS_POPUP,
            x, y, width, height, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
        if (!handle) throw std::runtime_error("fixture window");
    }
    void Show(COLORREF color) {
        SetWindowLongPtr(handle, GWLP_USERDATA, color);
        ShowWindow(handle, SW_SHOWNOACTIVATE);
        InvalidateRect(handle, nullptr, FALSE);
        UpdateWindow(handle);
    }
    RECT Bounds() const { RECT rect{}; GetWindowRect(handle, &rect); return rect; }
    ~Window() { if (handle) DestroyWindow(handle); }
};
struct XamlFixture {
    Window window{L"Ultrawide isolated XAML regression", 640, 96, 80, 80};
    Hosting::DesktopWindowXamlSource island;
    Controls::Grid host, background, buttons, tray;
    Controls::Grid button;
    Controls::Border plate;
    Root root;
    XamlFixture() {
        auto native = island.as<IDesktopWindowXamlSourceNative>();
        winrt::check_hresult(native->AttachToWindow(window.handle));
        HWND child{};
        winrt::check_hresult(native->get_WindowHandle(&child));
        SetWindowPos(child, nullptr, 0, 0, 640, 96,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        host.Width(640); host.Height(96);
        background.Width(640); background.Height(48);
        background.VerticalAlignment(VerticalAlignment::Top);
        buttons.Width(640); buttons.Height(48);
        buttons.VerticalAlignment(VerticalAlignment::Top);
        button.Width(64); button.Height(48);
        button.HorizontalAlignment(HorizontalAlignment::Left);
        plate.Name(L"BackgroundElement");
        plate.Background(Media::SolidColorBrush(winrt::Windows::UI::Color{255, 255, 0, 0}));
        plate.CornerRadius({8, 8, 8, 8});
        button.Children().Append(plate);
        buttons.Children().Append(button);
        host.Children().Append(background);
        host.Children().Append(buttons);
        host.Children().Append(tray);
        island.Content(host);
        window.Show(RGB(30, 30, 30));
        Pump(100);
        host.UpdateLayout();
        if (!Until([&] { return plate.ActualWidth() == 64 && plate.ActualHeight() == 48; }))
            throw std::runtime_error("XAML fixture did not arrange");
        root.dock.background = background;
    }
    void Prepare(DockInsets insets = {4, 4}) {
        PrepareStatePlates(root, buttons, tray, insets);
        Pump(10);
    }
    unsigned RimRed() {
        auto visual = Hosting::ElementCompositionPreview::GetElementChildVisual(plate);
        if (!visual) throw std::runtime_error("missing rim");
        auto brush = visual.as<ShapeVisual>().Shapes().GetAt(0)
            .as<CompositionSpriteShape>().StrokeBrush().as<CompositionLinearGradientBrush>();
        return brush.ColorStops().GetAt(0).Color().R;
    }
    double EffectiveRimAlpha() {
        auto visual = Hosting::ElementCompositionPreview::GetElementChildVisual(plate);
        if (!visual) return -1;
        auto brush = visual.as<ShapeVisual>().Shapes().GetAt(0)
            .as<CompositionSpriteShape>().StrokeBrush().as<CompositionLinearGradientBrush>();
        return brush.ColorStops().GetAt(0).Color().A * plate.Opacity();
    }
    ~XamlFixture() {
        RestoreStatePlates(root);
        island.Content(nullptr);
        island.Close();
    }
};

// Runs the production shader into a staging-readable target. No screen capture.
struct ShaderProbe {
    winrt::com_ptr<ID3D11Device> device;
    winrt::com_ptr<ID3D11DeviceContext> context;
    ShaderProbe() {
        D3D_FEATURE_LEVEL level{};
        winrt::check_hresult(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION,
            device.put(), &level, context.put()));
    }
    uint32_t Pixel(LiquidRenderer::LensConstants lens, unsigned x, unsigned y) {
        constexpr UINT width = 64, height = 48;
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = width; desc.Height = height; desc.MipLevels = 1; desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; desc.SampleDesc.Count = 1;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        uint32_t green[width * height];
        std::fill(std::begin(green), std::end(green), 0xff004000u);
        D3D11_SUBRESOURCE_DATA initial{green, width * 4, 0};
        winrt::com_ptr<ID3D11Texture2D> backdropTexture, target, staging;
        winrt::check_hresult(device->CreateTexture2D(&desc, &initial, backdropTexture.put()));
        winrt::com_ptr<ID3D11ShaderResourceView> source;
        winrt::check_hresult(device->CreateShaderResourceView(backdropTexture.get(), nullptr, source.put()));
        desc.BindFlags = D3D11_BIND_RENDER_TARGET;
        winrt::check_hresult(device->CreateTexture2D(&desc, nullptr, target.put()));
        winrt::com_ptr<ID3D11RenderTargetView> targetView;
        winrt::check_hresult(device->CreateRenderTargetView(target.get(), nullptr, targetView.put()));
        desc.BindFlags = 0; desc.Usage = D3D11_USAGE_STAGING; desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        winrt::check_hresult(device->CreateTexture2D(&desc, nullptr, staging.put()));
        auto compile = [&](const char* entry, const char* model) {
            winrt::com_ptr<ID3DBlob> shader, errors;
            std::string cap = std::to_string(kMaxButtonLenses);
            D3D_SHADER_MACRO macros[]{{"BUTTON_LENS_CAP", cap.c_str()}, {nullptr, nullptr}};
            HRESULT hr = D3DCompile(LiquidRenderer::kShader, sizeof(LiquidRenderer::kShader)-1,
                "regression-production-shader", macros, nullptr, entry, model,
                D3DCOMPILE_ENABLE_STRICTNESS, 0, shader.put(), errors.put());
            if (FAILED(hr) && errors) fprintf(stderr, "%.*s", int(errors->GetBufferSize()), static_cast<const char*>(errors->GetBufferPointer()));
            winrt::check_hresult(hr);
            return shader;
        };
        auto vsCode = compile("vs", "vs_5_0"), psCode = compile("ps", "ps_5_0");
        winrt::com_ptr<ID3D11VertexShader> vs;
        winrt::com_ptr<ID3D11PixelShader> ps;
        winrt::check_hresult(device->CreateVertexShader(vsCode->GetBufferPointer(), vsCode->GetBufferSize(), nullptr, vs.put()));
        winrt::check_hresult(device->CreatePixelShader(psCode->GetBufferPointer(), psCode->GetBufferSize(), nullptr, ps.put()));
        lens.imageSize[0] = width; lens.imageSize[1] = height;
        lens.center[0] = 32; lens.center[1] = 24;
        lens.halfSize[0] = 32; lens.halfSize[1] = 24; lens.radius = 0;
        lens.optics = {}; lens.optics.refraction = 0; lens.optics.chroma = 0;
        lens.optics.edgeHighlight = 0; lens.optics.specular = 0; lens.optics.fresnel = 0;
        lens.optics.opacity = 1; lens.optics.blur = 0; lens.lensRefraction = 0;
        lens.alertTintOpacity = 1; lens.lensBlend = 1;
        for (unsigned i = 0; i < lens.lensCount; ++i) {
            lens.lensTints[i][0] = 1; lens.lensTints[i][1] = 0;
            lens.lensTints[i][2] = 0; lens.lensTints[i][3] = 1;
        }
        D3D11_BUFFER_DESC cb{}; cb.ByteWidth = sizeof(lens); cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        D3D11_SUBRESOURCE_DATA cbData{&lens, 0, 0};
        winrt::com_ptr<ID3D11Buffer> constants;
        winrt::check_hresult(device->CreateBuffer(&cb, &cbData, constants.put()));
        D3D11_SAMPLER_DESC sd{}; sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.MaxLOD = D3D11_FLOAT32_MAX;
        winrt::com_ptr<ID3D11SamplerState> sampler;
        winrt::check_hresult(device->CreateSamplerState(&sd, sampler.put()));
        auto rtv = targetView.get(); auto srv = source.get(); auto constant = constants.get(); auto sample = sampler.get();
        D3D11_VIEWPORT viewport{0, 0, width, height, 0, 1};
        context->OMSetRenderTargets(1, &rtv, nullptr); context->OMSetBlendState(nullptr, nullptr, UINT_MAX);
        context->RSSetViewports(1, &viewport);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->VSSetShader(vs.get(), nullptr, 0); context->PSSetShader(ps.get(), nullptr, 0);
        context->PSSetShaderResources(0, 1, &srv); context->PSSetSamplers(0, 1, &sample);
        context->PSSetConstantBuffers(0, 1, &constant); context->Draw(3, 0);
        context->CopyResource(staging.get(), target.get());
        D3D11_MAPPED_SUBRESOURCE mapped{};
        winrt::check_hresult(context->Map(staging.get(), 0, D3D11_MAP_READ, 0, &mapped));
        uint32_t pixel = reinterpret_cast<uint32_t*>(static_cast<BYTE*>(mapped.pData) + y * mapped.RowPitch)[x];
        context->Unmap(staging.get(), 0);
        context->ClearState();
        return pixel;
    }
};
void PlateChecks() {
    XamlFixture fixture;
    g_keepButtonRim = true; g_buttonCornerRadius = 12; g_buttonVerticalPadding = 0;
    fixture.host.RequestedTheme(ElementTheme::Dark); Pump(30);
    g_theme = DockTheme::Native;
    auto initialLocal = fixture.plate.ReadLocalValue(UIElement::OpacityProperty());
    fixture.Prepare();
    Check(fixture.plate.ReadLocalValue(UIElement::OpacityProperty()) == initialLocal,
        "L03 Native leaves opacity ownership unchanged");
    Check(std::abs(fixture.EffectiveRimAlpha() - 26) < 1.5,
        "V01 Native rim retains reference effective strength");
    RestoreStatePlates(fixture.root);
    g_theme = DockTheme::Glass;
    fixture.Prepare();
    Check(std::abs(fixture.EffectiveRimAlpha() - 26) < 1.5,
        "V01 Glass rim compensates only its real dimming");
    fixture.plate.Opacity(0.71);
    fixture.Prepare();
    Check(std::abs(fixture.plate.Opacity() - 0.71) < 0.001,
        "L03 refresh preserves a later opacity writer");
    RestoreStatePlates(fixture.root);
    Check(std::abs(fixture.plate.Opacity() - 0.71) < 0.001,
        "L03 restoration preserves the later opacity writer");
    fixture.plate.ClearValue(UIElement::OpacityProperty());
    fixture.Prepare();
    Check(fixture.RimRed() == 255, "V05 dark rim is white");
    fixture.host.RequestedTheme(ElementTheme::Light); Pump(30);
    fixture.Prepare();
    Check(fixture.RimRed() == 0, "V05 same plate changes to light-theme rim");
    RestoreStatePlates(fixture.root);

    g_theme = DockTheme::Liquid; g_buttonLiquidLayer = true;
    g_buttonCornerRadius = 0;
    fixture.Prepare({12, 12});
    auto visual = Hosting::ElementCompositionPreview::GetElementVisual(fixture.button);
    fixture.root.sources.push_back({CanonicalIdentity(fixture.button), winrt::make_weak<FrameworkElement>(fixture.button), visual, Root::Source::Life::Active});
#ifndef UW_BASELINE
    fixture.root.sources.back().included = true;
    fixture.root.sources.back().plate = fixture.plate;
#endif
    LiquidRenderer::LensConstants active{};
    AddButtonLenses(fixture.root, active, 1);
    ShaderProbe shader;
    // One pixel inside the inset corner: fully inside the dock and both clip
    // edges, so neither anti-aliased edge dilutes the lens coverage.
    auto corner = shader.Pixel(active, 1, 13);
    printf("V03 sampled pixel=%08x\n", corner);
    auto interior = shader.Pixel(active, 32, 24);
    unsigned cornerRed = (corner >> 16) & 255;
    unsigned interiorRed = (interior >> 16) & 255;
    Check(interiorRed > 0 && std::abs(int(cornerRed) - int(interiorRed)) <= 2,
        "V03 inset-square corner receives button lens coverage");
    fixture.root.sources.back().life = Root::Source::Life::Retired;
    LiquidRenderer::LensConstants retired{};
    AddButtonLenses(fixture.root, retired, 1);
    auto retiredPixel = shader.Pixel(retired, 32, 24);
    Check(((retiredPixel >> 16) & 255) == 0,
        "V02 retired button no longer tints the rendered backdrop");
}

void SchedulerChecks() {
    XamlFixture fixture;
    auto renderer = std::make_shared<LiquidRenderer>();
    auto compositor = Hosting::ElementCompositionPreview::GetElementVisual(fixture.background).Compositor();
    if (!renderer->Initialize(compositor, fixture.window.handle, 640, 48))
        throw std::runtime_error("renderer initialization");
    LiquidRenderer::LensConstants lens{};
    lens.center[0] = 320; lens.center[1] = 24; lens.halfSize[0] = 280; lens.halfSize[1] = 20; lens.radius = 12;
    RECT rect{80, 80, 720, 128};
    Check(renderer->UpdateGeometry(rect, 640, 48, lens), "renderer accepts initial geometry");
    Until([&] { std::lock_guard lock(renderer->mutex); return renderer->lastPresent.time_since_epoch().count() != 0; });
    {
        std::lock_guard lock(renderer->mutex);
        renderer->lastPresent = std::chrono::steady_clock::now();
    }
    lens.radius = 11;
    Check(renderer->UpdateGeometry(rect, 640, 48, lens), "renderer accepts final throttled geometry");
    Check(Until([&] { std::lock_guard lock(renderer->mutex); return !renderer->pendingPresent; }, 90),
        "C05 final geometry presents without a flush or another producer");
    Check(renderer->UpdateGeometry({80, 80, 848, 128}, 768, 48, lens),
        "E03 cached render views permit swap-chain resizing");
    renderer->Stop();
    Pump(30);
    Check(SUCCEEDED(renderer->Failure()), "scheduler shutdown and resize retain a healthy device");
}
#ifndef UW_BASELINE
uint32_t TexturePixel(LiquidRenderer& renderer, ID3D11Texture2D* texture,
                      UINT x, UINT y) {
    D3D11_TEXTURE2D_DESC description{};
    texture->GetDesc(&description);
    if (x >= description.Width || y >= description.Height)
        throw std::runtime_error("pixel outside valid capture");
    description.Width = description.Height = 1;
    description.BindFlags = 0;
    description.MiscFlags = 0;
    description.Usage = D3D11_USAGE_STAGING;
    description.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    winrt::com_ptr<ID3D11Texture2D> staging;
    winrt::check_hresult(renderer.device->CreateTexture2D(
        &description, nullptr, staging.put()));
    D3D11_BOX box{x, y, 0, x + 1, y + 1, 1};
    renderer.context->CopySubresourceRegion(
        staging.get(), 0, 0, 0, 0, texture, 0, &box);
    D3D11_MAPPED_SUBRESOURCE mapped{};
    winrt::check_hresult(renderer.context->Map(
        staging.get(), 0, D3D11_MAP_READ, 0, &mapped));
    uint32_t pixel = *static_cast<uint32_t*>(mapped.pData);
    renderer.context->Unmap(staging.get(), 0);
    return pixel;
}
bool NearColor(uint32_t pixel, uint32_t expected) {
    for (int shift : {0, 8, 16, 24})
        if (std::abs(int((pixel >> shift) & 255) -
                     int((expected >> shift) & 255)) > 2) return false;
    return true;
}
void CaptureChecks() {
    XamlFixture fixture;
    Window red(L"Ultrawide synthetic RED capture", 320, 96, 80, 80);
    Window blue(L"Ultrawide synthetic BLUE capture", 320, 96, 180, 80);
    red.Show(RGB(255, 0, 0)); blue.Show(RGB(0, 0, 255));
    auto renderer = std::make_shared<LiquidRenderer>();
    auto compositor = Hosting::ElementCompositionPreview::GetElementVisual(fixture.background).Compositor();
    if (!renderer->Initialize(compositor, fixture.window.handle, 640, 48))
        throw std::runtime_error("capture renderer initialization");
    LiquidRenderer::LensConstants lens{};
    lens.center[0] = 320; lens.center[1] = 24;
    lens.halfSize[0] = 320; lens.halfSize[1] = 24;
    renderer->UpdateGeometry({80, 80, 720, 128}, 640, 48, lens);
    // Never call desktop discovery: only these two owned synthetic HWNDs.
    renderer->UpdateSources({{red.handle, red.Bounds()}, {blue.handle, blue.Bounds()}});
    std::shared_ptr<LiquidRenderer::Source> redSource, blueSource;
    {
        std::lock_guard lock(renderer->mutex);
        for (auto const& source : renderer->sources) {
            if (source->window == red.handle) redSource = source;
            if (source->window == blue.handle) blueSource = source;
        }
    }
    if (!redSource || !blueSource) throw std::runtime_error("fixture capture unavailable");
    auto sourceColor = [&](auto const& source, UINT x, UINT y, uint32_t color) {
        std::lock_guard lock(renderer->mutex);
        return source->latest && source->textureSize.Width > int(x) &&
               source->textureSize.Height > int(y) &&
               NearColor(TexturePixel(*renderer, source->latest.get(), x, y), color);
    };
    Check(Until([&] {
        return sourceColor(redSource, 16, 16, 0xffff0000) &&
               sourceColor(blueSource, 16, 16, 0xff0000ff);
    }), "C03 first capture frames reach their published sources");

    RECT patch{32, 20, 64, 40};
    SetWindowLongPtr(red.handle, GWLP_USERDATA, RGB(0, 0, 255));
    InvalidateRect(red.handle, &patch, FALSE); UpdateWindow(red.handle);
    Check(Until([&] {
        return sourceColor(redSource, 48, 30, 0xff0000ff) &&
               sourceColor(redSource, 16, 16, 0xffff0000);
    }), "E01 partial repaint updates changed pixels and preserves untouched content");
    SetWindowPos(red.handle, nullptr, 80, 80, 520, 160, SWP_NOZORDER | SWP_NOACTIVATE);
    red.Show(RGB(0, 255, 0));
    // Mirrors the 250 ms discovery timer: a grown source whose surface never
    // delivered another frame must be recycled by the next scene update.
    bool grew = Until([&] {
        if (sourceColor(redSource, 510, 150, 0xff00ff00)) return true;
        renderer->UpdateSources({{red.handle, red.Bounds()}, {blue.handle, blue.Bounds()}});
        std::lock_guard lock(renderer->mutex);
        for (auto const& source : renderer->sources)
            if (source->window == red.handle) redSource = source;
        return false;
    });
    {
        std::lock_guard lock(renderer->mutex);
        printf("C01 grow pool=%dx%d content=%dx%d texture=%dx%d\n",
            redSource->poolSize.Width, redSource->poolSize.Height,
            redSource->contentSize.Width, redSource->contentSize.Height,
            redSource->textureSize.Width, redSource->textureSize.Height);
    }
    Check(grew, "C01 growing capture exposes newly allocated far-edge pixels");
    SetWindowPos(red.handle, nullptr, 80, 80, 160, 64, SWP_NOZORDER | SWP_NOACTIVATE);
    red.Show(RGB(0, 255, 255));
    Check(Until([&] {
        std::lock_guard lock(renderer->mutex);
        return redSource->latest && redSource->textureSize.Width == 160 &&
               redSource->textureSize.Height == 64 &&
               NearColor(TexturePixel(*renderer, redSource->latest.get(), 150, 55), 0xff00ffff);
    }), "C01 shrinking capture excludes stale surface padding");
    {
        std::lock_guard lock(renderer->mutex);
        redSource->latestView = nullptr;
    }
    red.Show(RGB(255, 0, 0));
    Check(Until([&] {
        std::lock_guard lock(renderer->mutex);
        return redSource->latestView && redSource->latest &&
               NearColor(TexturePixel(*renderer, redSource->latest.get(), 16, 16), 0xffff0000);
    }), "C04 missing view recovers on the next same-size frame");

    LiquidRenderer::CloseSource(redSource);
    LiquidRenderer::CloseSource(blueSource);
    auto sceneColor = [&](UINT x, uint32_t color) {
        std::lock_guard lock(renderer->mutex);
        return !renderer->pendingPresent &&
               NearColor(TexturePixel(*renderer, renderer->composite.get(), x, 20), color);
    };
    renderer->UpdateSources({{red.handle, red.Bounds()}, {blue.handle, blue.Bounds()}});
    Check(Until([&] { return sceneColor(130, 0xffff0000); }), "C02 initial overlap respects source order");
    renderer->UpdateSources({{blue.handle, blue.Bounds()}, {red.handle, red.Bounds()}});
    Check(Until([&] { return sceneColor(130, 0xff0000ff); }),
        "C02 reorder presents without another capture frame");
    SetWindowPos(blue.handle, nullptr, 400, 80, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    renderer->UpdateSources({{blue.handle, blue.Bounds()}, {red.handle, red.Bounds()}});
    Check(Until([&] { return sceneColor(130, 0xffff0000) && sceneColor(370, 0xff0000ff); }),
        "C02 static movement restores uncovered content and retains moved pixels");
    renderer->UpdateSources({{blue.handle, blue.Bounds()}});
    Check(Until([&] { return sceneColor(130, 0); }),
        "C02 removal clears stale pixels without another capture frame");
    lens.radius = 7;
    renderer->UpdateGeometry({80, 80, 720, 128}, 640, 48, lens);
    renderer->Stop();
    Pump(30);
    Check(SUCCEEDED(renderer->Failure()), "capture teardown drains pending presentation safely");
}
void MotionChecks() {
    XamlFixture fixture;
    g_theme = DockTheme::Liquid;
    auto visual = Hosting::ElementCompositionPreview::GetElementVisual(fixture.button);
    auto animation = visual.Compositor().CreateScalarKeyFrameAnimation();
    animation.InsertKeyFrame(0, 0);
    animation.InsertKeyFrame(1, 160);
    animation.Duration(std::chrono::milliseconds(400));
    animation.Target(L"Translation.X");
    g_roots.emplace_back();
    g_roots.back().frame = fixture.host;
    g_roots.back().measurementsReady = true;
    auto batch = BeginLiquidMotionBatch(&animation);
    fixture.button.StartAnimation(animation);
    EndLiquidMotionBatch(batch);
    for (int i = 0; i < 6; ++i) {
        Pump(65);
        printf("MOTION t=%d xaml=%f visual=%f batches=%zu\n", (i + 1) * 65,
            fixture.button.TransformToVisual(fixture.background).TransformPoint({0, 0}).X,
            visual.Offset().x, g_liquidMotionBatches.size());
    }
    Check(Until([] { return g_liquidMotionBatches.empty(); }),
        "V04 finite native animation observation completes without a fixed timeout");
    fixture.button.StopAnimation(animation);
    StopLiquidMotionTracking();
    g_roots.clear();
}
#endif
}
int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    try {
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        winrt::init_apartment(winrt::apartment_type::single_threaded);
        auto manager = Hosting::WindowsXamlManager::InitializeForCurrentThread();
        g_ownerThread = GetCurrentThreadId();
        SetWindowPos_Original = SetWindowPos;
        DestroyWindow_Original = DestroyWindow;
        std::string_view mode = argc > 1 ? argv[1] : "all";
        if (mode == "all" || mode == "plates") regression::PlateChecks();
        if (mode == "all" || mode == "scheduler") regression::SchedulerChecks();
#ifndef UW_BASELINE
        if (mode == "all" || mode == "capture") regression::CaptureChecks();
        if (mode == "all" || mode == "motion") regression::MotionChecks();
#endif
        regression::Pump(30);
        manager.Close();
        printf("RESULT: %d failures\n", regression::failures);
        return regression::failures ? 1 : 0;
    } catch (winrt::hresult_error const& error) {
        printf("HRESULT %08x: %ls\n", unsigned(error.code().value), error.message().c_str());
        return 2;
    } catch (std::exception const& error) {
        printf("ERROR: %s\n", error.what());
        return 2;
    }
}
