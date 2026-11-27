// ==WindhawkMod==
// @id              focusglass
// @name            FocusGlass
// @description     macOS-style GPU background blur for overlapping desktop windows
// @version         0.1.0
// @author          SAYON BISWAS
// @github          https://github.com/sayon-coder
// @homepage        https://github.com/sayon-coder/macos-focus-blur
// @include         windhawk.exe
// @compilerOptions -ld3d11 -ldxgi -ldcomp -ld3dcompiler -ldwmapi -lwindowsapp -lruntimeobject -lshell32 -lole32 -luuid -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# FocusGlass

An independent Windhawk Tool Mod which applies a macOS-inspired blurred backdrop
behind the active application window when it overlaps another application window.

## Features

- GPU-only Direct3D 11 downsample and separable Gaussian blur.
- DirectComposition premultiplied-alpha overlay.
- Blur only when a lower-z-order application window overlaps the active window.
  Side-by-side windows stay sharp.
- Rounded active-window cutout, clear taskbars and owned popups, plus a fixed
  30-pixel top safe area reserved for desktop menu bars.
- Soft foreground shadow.
- Click-through overlay and screen-capture exclusion.
- Tray icon and global hotkeys.

## Controls

- Tray left-click or Ctrl+Alt+B: pause/resume.
- Tray right-click: blur controls.
- Ctrl+Alt+Up / Ctrl+Alt+Down: adjust blur strength (1–5).
- Disable or unload the mod from Windhawk to stop it.

The mod runs as a dedicated `windhawk.exe` Tool Mod process on Windhawk 1.7.3.
It doesn't inject the renderer into Explorer. Windhawk controls process startup
and shutdown; no separate Startup shortcut is needed.

## Settings

**Blur strength on startup** chooses the initial level (1–5). The tray menu and
hotkeys can adjust it for the current run. **Start paused** controls whether
capture starts immediately when Windhawk loads the mod.

## Compatibility and limitations

- Intended for Windows 10 version 2004 or later and Windows 11.
- Validated by the author on Windows 11 with Windhawk 1.7.3 stable.
- Tested by the author with the x86 `windhawk.exe` host on Windhawk 1.7.3.
  Other host architectures have not yet been verified.
- The blur currently captures the primary monitor only.
- Restart the mod after changing the display topology or display resolution.
- Requires a GPU driver supporting Direct3D 11, DirectComposition and Windows
  Graphics Capture.

## Startup troubleshooting

The mod must be allowed to load into `windhawk.exe`. If you have configured a
restrictive global Process exclusion list in Windhawk, add `windhawk.exe` to the
Process inclusion list so the Tool Mod launcher can start.

## Privacy

Desktop frames are captured and processed locally. The mod does not save captured
frames to disk or transmit them over the network. Installing it through Windhawk
is self-contained: the standalone `FocusGlass.exe` and development repository
are not required.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- BlurLevel: 1
  $name: Blur strength on startup
  $description: Initial GPU blur level from 1 (lightest) to 5 (strongest).
- StartPaused: false
  $name: Start paused
  $description: Start the tool without capturing the desktop until resumed.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <dcomp.h>
#include <inspectable.h>
#include <windows.graphics.capture.interop.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>

#include <windhawk_api.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

namespace focusglass
{
    namespace detail
    {
        inline void CheckHresult(HRESULT hr, const char* expression)
        {
            if (SUCCEEDED(hr))
                return;

            char code[16]{};
            std::snprintf(code, sizeof(code), "0x%08X", static_cast<unsigned int>(hr));
            throw std::runtime_error(std::string(expression) +
                " failed with HRESULT " + code + ".");
        }
    }

#define FOCUSGLASS_CHECK_HRESULT(expression) \
    ::focusglass::detail::CheckHresult((expression), #expression)

    class GpuCompositor
    {
    public:
        GpuCompositor(ID3D11Device* device, ID3D11DeviceContext* context,
            HWND targetWindow, int outputWidth, int outputHeight)
            : m_outputWidth(outputWidth), m_outputHeight(outputHeight)
        {
            if (!device || !context || !targetWindow || outputWidth <= 0 || outputHeight <= 0)
                throw std::invalid_argument("GPU compositor received invalid initialization parameters.");

            m_device.copy_from(device);
            m_context.copy_from(context);

            CreateShaders();
            CreateSampler();
            CreateConstantBuffers();
            CreateCompositionSwapChain(targetWindow);
            CreateCompositionVisual(targetWindow);
        }

        GpuCompositor(const GpuCompositor&) = delete;
        GpuCompositor& operator=(const GpuCompositor&) = delete;

        void Render(ID3D11Texture2D* capturedFrame, const RECT& monitorBounds,
            const std::vector<RECT>& clearRegions, int blurLevel)
        {
            if (!capturedFrame || clearRegions.empty())
                return;

            EnsureFrameResources(capturedFrame);
            m_context->CopyResource(m_sourceCopy.get(), capturedFrame);

            BlurConstants blur{};
            const int level = std::clamp(blurLevel, 1, 5);
            blur.sigma = 1.2f + static_cast<float>(level - 1) * 0.64f;

            blur.texelStep[0] = 1.0f / static_cast<float>(m_blurWidth);
            blur.texelStep[1] = 0.0f;
            m_context->UpdateSubresource(m_blurConstants.get(), 0, nullptr, &blur, 0, 0);
            DrawPass(m_downsamplePixelShader.get(), m_sourceSrv.get(), m_downsampleTarget.rtv.get(),
                m_blurWidth, m_blurHeight, nullptr, true);

            // Horizontal pass: downsampled source -> horizontal intermediate.
            DrawPass(m_blurPixelShader.get(), m_downsampleTarget.srv.get(), m_horizontalTarget.rtv.get(),
                m_blurWidth, m_blurHeight, m_blurConstants.get(), false);

            // Vertical pass: horizontal intermediate -> final blurred texture.
            blur.texelStep[0] = 0.0f;
            blur.texelStep[1] = 1.0f / static_cast<float>(m_blurHeight);
            m_context->UpdateSubresource(m_blurConstants.get(), 0, nullptr, &blur, 0, 0);
            DrawPass(m_blurPixelShader.get(), m_horizontalTarget.srv.get(), m_blurTarget.rtv.get(),
                m_blurWidth, m_blurHeight, m_blurConstants.get(), false);

            CompositeConstants composite{};
            composite.outputParams[0] = static_cast<float>(m_outputWidth);
            composite.outputParams[1] = static_cast<float>(m_outputHeight);
            composite.outputParams[2] = 0.93f; // Keep the restrained backdrop brightness from v0.1.x.
            composite.outputParams[3] = 10.0f;  // Rounded foreground cutout corner radius.
            composite.foregroundRect = ToLocalRect(clearRegions.front(), monitorBounds);

            // shadowParams: vertical offset, Gaussian sigma, opacity, reserved.
            composite.shadowParams[0] = 6.0f;
            composite.shadowParams[1] = 16.0f;
            composite.shadowParams[2] = 0.27f;
            composite.shadowParams[3] = 0.0f;

            const size_t availableClearRects = clearRegions.size() > 1 ? clearRegions.size() - 1 : 0;
            composite.clearCount = static_cast<UINT>(std::min<size_t>(availableClearRects, static_cast<size_t>(kMaxClearRects)));
            for (UINT i = 0; i < composite.clearCount; ++i)
                composite.clearRects[i] = ToLocalRect(clearRegions[i + 1], monitorBounds);

            m_context->UpdateSubresource(m_compositeConstants.get(), 0, nullptr, &composite, 0, 0);

            // Follow the previously validated composition path: acquire the current
            // flip-model buffer and create its RTV for this frame. Avoid bulk RTV
            // creation during startup, which returned E_INVALIDARG on the target PC.
            const UINT currentBackBufferIndex = m_swapChain3->GetCurrentBackBufferIndex();
            winrt::com_ptr<ID3D11Texture2D> currentBackBuffer;
            winrt::com_ptr<ID3D11RenderTargetView> currentBackBufferRtv;
            FOCUSGLASS_CHECK_HRESULT(m_swapChain->GetBuffer(
                currentBackBufferIndex, __uuidof(ID3D11Texture2D), currentBackBuffer.put_void()));
            FOCUSGLASS_CHECK_HRESULT(m_device->CreateRenderTargetView(
                currentBackBuffer.get(), nullptr, currentBackBufferRtv.put()));
            ID3D11RenderTargetView* target = currentBackBufferRtv.get();
            m_context->OMSetRenderTargets(1, &target, nullptr);
            D3D11_VIEWPORT viewport{};
            viewport.Width = static_cast<float>(m_outputWidth);
            viewport.Height = static_cast<float>(m_outputHeight);
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;
            m_context->RSSetViewports(1, &viewport);
            m_context->IASetInputLayout(nullptr);
            m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_context->VSSetShader(m_vertexShader.get(), nullptr, 0);
            m_context->PSSetShader(m_compositePixelShader.get(), nullptr, 0);

            ID3D11ShaderResourceView* sourceView = m_blurTarget.srv.get();
            m_context->PSSetShaderResources(0, 1, &sourceView);
            ID3D11SamplerState* sampler = m_sampler.get();
            m_context->PSSetSamplers(0, 1, &sampler);
            ID3D11Buffer* constants = m_compositeConstants.get();
            m_context->PSSetConstantBuffers(0, 1, &constants);
            m_context->Draw(3, 0);

            ID3D11ShaderResourceView* nullView = nullptr;
            m_context->PSSetShaderResources(0, 1, &nullView);
            m_context->OMSetRenderTargets(0, nullptr, nullptr);

            const HRESULT presentResult = m_swapChain->Present(1, 0);
            if (FAILED(presentResult))
                FOCUSGLASS_CHECK_HRESULT(presentResult);
        }

    private:
        static constexpr UINT kMaxClearRects = 32;

        struct RenderTarget
        {
            winrt::com_ptr<ID3D11Texture2D> texture;
            winrt::com_ptr<ID3D11ShaderResourceView> srv;
            winrt::com_ptr<ID3D11RenderTargetView> rtv;
        };

        struct BlurConstants
        {
            float texelStep[2]{};
            float sigma{};
            float padding{};
        };

        struct CompositeConstants
        {
            float outputParams[4]{};
            std::array<float, 4> foregroundRect{};
            float shadowParams[4]{};
            UINT clearCount{};
            float padding[3]{};
            std::array<std::array<float, 4>, kMaxClearRects> clearRects{};
        };

        static std::array<float, 4> ToLocalRect(const RECT& rect, const RECT& monitor)
        {
            return {
                static_cast<float>(rect.left - monitor.left),
                static_cast<float>(rect.top - monitor.top),
                static_cast<float>(rect.right - monitor.left),
                static_cast<float>(rect.bottom - monitor.top)
            };
        }

        static constexpr const char* ShaderSource()
        {
            return R"(
Texture2D SourceTexture : register(t0);
SamplerState LinearSampler : register(s0);

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VertexOutput VSMain(uint vertexId : SV_VertexID)
{
    VertexOutput output;
    if (vertexId == 0)
    {
        output.position = float4(-1.0,  1.0, 0.0, 1.0);
        output.uv = float2(0.0, 0.0);
    }
    else if (vertexId == 1)
    {
        output.position = float4( 3.0,  1.0, 0.0, 1.0);
        output.uv = float2(2.0, 0.0);
    }
    else
    {
        output.position = float4(-1.0, -3.0, 0.0, 1.0);
        output.uv = float2(0.0, 2.0);
    }
    return output;
}

float4 PSDownsample(VertexOutput input) : SV_TARGET
{
    uint sourceWidth, sourceHeight;
    SourceTexture.GetDimensions(sourceWidth, sourceHeight);
    float2 texel = 1.0 / float2(sourceWidth, sourceHeight);
    float4 accumulated = 0.0;

    // A 3x3 prefilter reduces shimmer before downsampling the monitor image.
    [unroll]
    for (int y = -1; y <= 1; ++y)
    {
        [unroll]
        for (int x = -1; x <= 1; ++x)
            accumulated += SourceTexture.SampleLevel(LinearSampler, input.uv + texel * float2(x, y), 0);
    }

    return accumulated / 9.0;
}

cbuffer BlurParams : register(b0)
{
    float2 texelStep;
    float sigma;
    float blurPadding;
};

float4 PSBlur(VertexOutput input) : SV_TARGET
{
    float4 accumulated = 0.0;
    float totalWeight = 0.0;
    float safeSigma = max(sigma, 0.5);

    [unroll]
    for (int tap = -12; tap <= 12; ++tap)
    {
        float sampleWeight = exp(-0.5 * (tap * tap) / (safeSigma * safeSigma));
        float2 sampleUv = input.uv + texelStep * tap;
        accumulated += SourceTexture.SampleLevel(LinearSampler, sampleUv, 0) * sampleWeight;
        totalWeight += sampleWeight;
    }

    return accumulated / max(totalWeight, 0.0001);
}

cbuffer CompositeParams : register(b0)
{
    float4 outputParams;       // width, height, brightness, foreground corner radius
    float4 foregroundRect;     // left, top, right, bottom in monitor-local pixels
    float4 shadowParams;       // vertical offset, sigma, opacity, reserved
    uint clearCount;
    float3 compositePadding;
    float4 clearRects[32];
};

float RoundedRectDistance(float2 samplePosition, float4 boundsRect, float radius)
{
    float2 center = (boundsRect.xy + boundsRect.zw) * 0.5;
    float2 halfSize = max((boundsRect.zw - boundsRect.xy) * 0.5, float2(0.0, 0.0));
    float2 q = abs(samplePosition - center) - (halfSize - radius);
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - radius;
}

float4 PSComposite(VertexOutput input) : SV_TARGET
{
    float2 pixel = input.uv * outputParams.xy;

    // Transparent cutout for the active window. The real window remains live and sharp.
    float foregroundDistance = RoundedRectDistance(pixel, foregroundRect, outputParams.w);
    if (foregroundDistance <= 0.0)
        return float4(0.0, 0.0, 0.0, 0.0);

    // Preserve only owned popups, taskbars and the configured top menu-bar safe area.
    [loop]
    for (uint index = 0; index < clearCount; ++index)
    {
        float4 rect = clearRects[index];
        if (pixel.x >= rect.x && pixel.x < rect.z && pixel.y >= rect.y && pixel.y < rect.w)
            return float4(0.0, 0.0, 0.0, 0.0);
    }

    float4 color = SourceTexture.SampleLevel(LinearSampler, input.uv, 0);

    // A medium soft shadow sits over the blurred backdrop and just outside the foreground.
    float4 shadowRect = foregroundRect + float4(0.0, shadowParams.x, 0.0, shadowParams.x);
    float shadowDistance = max(0.0, RoundedRectDistance(pixel, shadowRect, outputParams.w));
    float shadow = shadowParams.z * exp(
        -0.5 * (shadowDistance * shadowDistance) /
        max(1.0, shadowParams.y * shadowParams.y));

    color.rgb *= outputParams.z * (1.0 - shadow);
    return float4(color.rgb, 1.0);
}
)";
        }

        void CompileShader(const char* entryPoint, const char* profile,
            winrt::com_ptr<ID3DBlob>& bytecode)
        {
            winrt::com_ptr<ID3DBlob> diagnostics;
            const char* source = ShaderSource();
            const HRESULT hr = D3DCompile(source, std::strlen(source), "FocusGlassGpu.hlsl",
                nullptr, nullptr, entryPoint, profile, D3DCOMPILE_ENABLE_STRICTNESS,
                0, bytecode.put(), diagnostics.put());

            if (FAILED(hr))
            {
                std::string message = std::string("Could not compile GPU shader ") + entryPoint + ".";
                if (diagnostics && diagnostics->GetBufferPointer())
                {
                    message += " ";
                    message.append(static_cast<const char*>(diagnostics->GetBufferPointer()),
                        diagnostics->GetBufferSize());
                }
                throw std::runtime_error(message);
            }
        }

        void CreateShaders()
        {
            winrt::com_ptr<ID3DBlob> bytecode;
            CompileShader("VSMain", "vs_4_0", bytecode);
            FOCUSGLASS_CHECK_HRESULT(m_device->CreateVertexShader(bytecode->GetBufferPointer(),
                bytecode->GetBufferSize(), nullptr, m_vertexShader.put()));

            bytecode = nullptr;
            CompileShader("PSDownsample", "ps_4_0", bytecode);
            FOCUSGLASS_CHECK_HRESULT(m_device->CreatePixelShader(bytecode->GetBufferPointer(),
                bytecode->GetBufferSize(), nullptr, m_downsamplePixelShader.put()));

            bytecode = nullptr;
            CompileShader("PSBlur", "ps_4_0", bytecode);
            FOCUSGLASS_CHECK_HRESULT(m_device->CreatePixelShader(bytecode->GetBufferPointer(),
                bytecode->GetBufferSize(), nullptr, m_blurPixelShader.put()));

            bytecode = nullptr;
            CompileShader("PSComposite", "ps_4_0", bytecode);
            FOCUSGLASS_CHECK_HRESULT(m_device->CreatePixelShader(bytecode->GetBufferPointer(),
                bytecode->GetBufferSize(), nullptr, m_compositePixelShader.put()));
        }

        void CreateSampler()
        {
            D3D11_SAMPLER_DESC desc{};
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            desc.MinLOD = 0.0f;
            desc.MaxLOD = D3D11_FLOAT32_MAX;
            FOCUSGLASS_CHECK_HRESULT(m_device->CreateSamplerState(&desc, m_sampler.put()));
        }

        void CreateConstantBuffers()
        {
            D3D11_BUFFER_DESC desc{};
            desc.ByteWidth = sizeof(BlurConstants);
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            FOCUSGLASS_CHECK_HRESULT(m_device->CreateBuffer(&desc, nullptr, m_blurConstants.put()));

            desc.ByteWidth = sizeof(CompositeConstants);
            FOCUSGLASS_CHECK_HRESULT(m_device->CreateBuffer(&desc, nullptr, m_compositeConstants.put()));
        }

        void CreateCompositionSwapChain(HWND targetWindow)
        {
            winrt::com_ptr<IDXGIDevice> dxgiDevice;
            FOCUSGLASS_CHECK_HRESULT(m_device->QueryInterface(__uuidof(IDXGIDevice), dxgiDevice.put_void()));

            winrt::com_ptr<IDXGIAdapter> adapter;
            FOCUSGLASS_CHECK_HRESULT(dxgiDevice->GetAdapter(adapter.put()));

            winrt::com_ptr<IDXGIFactory2> factory;
            FOCUSGLASS_CHECK_HRESULT(adapter->GetParent(__uuidof(IDXGIFactory2), factory.put_void()));

            DXGI_SWAP_CHAIN_DESC1 desc{};
            desc.Width = static_cast<UINT>(m_outputWidth);
            desc.Height = static_cast<UINT>(m_outputHeight);
            desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            desc.Stereo = FALSE;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            desc.BufferCount = 2;
            desc.Scaling = DXGI_SCALING_STRETCH;
            desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
            desc.Flags = 0;

            FOCUSGLASS_CHECK_HRESULT(factory->CreateSwapChainForComposition(
                m_device.get(), &desc, nullptr, m_swapChain.put()));

            // IDXGISwapChain3 exposes the current writable back-buffer index on Windows 10+.
            FOCUSGLASS_CHECK_HRESULT(m_swapChain->QueryInterface(__uuidof(IDXGISwapChain3),
                m_swapChain3.put_void()));

            // Keep the default DXGI frame queue. SetMaximumFrameLatency requires
            // DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT, which this composition
            // swap chain intentionally does not enable.
            winrt::com_ptr<ID3D11Texture2D> backBuffer;
            FOCUSGLASS_CHECK_HRESULT(m_swapChain->GetBuffer(
                0, __uuidof(ID3D11Texture2D), backBuffer.put_void()));
            FOCUSGLASS_CHECK_HRESULT(m_device->CreateRenderTargetView(
                backBuffer.get(), nullptr, m_swapChainTarget.put()));
            const float transparent[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            m_context->ClearRenderTargetView(m_swapChainTarget.get(), transparent);
            FOCUSGLASS_CHECK_HRESULT(m_swapChain->Present(1, 0));
            m_swapChainTarget = nullptr;

            (void)targetWindow;
        }

        void CreateCompositionVisual(HWND targetWindow)
        {
            winrt::com_ptr<IDXGIDevice> dxgiDevice;
            FOCUSGLASS_CHECK_HRESULT(m_device->QueryInterface(__uuidof(IDXGIDevice), dxgiDevice.put_void()));
            FOCUSGLASS_CHECK_HRESULT(DCompositionCreateDevice(dxgiDevice.get(),
                __uuidof(IDCompositionDevice), m_compositionDevice.put_void()));
            FOCUSGLASS_CHECK_HRESULT(m_compositionDevice->CreateTargetForHwnd(
                targetWindow, TRUE, m_compositionTarget.put()));
            FOCUSGLASS_CHECK_HRESULT(m_compositionDevice->CreateVisual(m_visual.put()));
            FOCUSGLASS_CHECK_HRESULT(m_visual->SetContent(m_swapChain.get()));
            FOCUSGLASS_CHECK_HRESULT(m_compositionTarget->SetRoot(m_visual.get()));
            FOCUSGLASS_CHECK_HRESULT(m_compositionDevice->Commit());
        }

        void CreateRenderTarget(UINT width, UINT height, RenderTarget& target)
        {
            D3D11_TEXTURE2D_DESC desc{};
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            FOCUSGLASS_CHECK_HRESULT(m_device->CreateTexture2D(&desc, nullptr, target.texture.put()));
            FOCUSGLASS_CHECK_HRESULT(m_device->CreateShaderResourceView(target.texture.get(), nullptr, target.srv.put()));
            FOCUSGLASS_CHECK_HRESULT(m_device->CreateRenderTargetView(target.texture.get(), nullptr, target.rtv.put()));
        }

        void EnsureFrameResources(ID3D11Texture2D* capturedFrame)
        {
            D3D11_TEXTURE2D_DESC sourceDesc{};
            capturedFrame->GetDesc(&sourceDesc);
            if (sourceDesc.Width == 0 || sourceDesc.Height == 0 ||
                sourceDesc.ArraySize != 1 || sourceDesc.SampleDesc.Count != 1)
                throw std::runtime_error("Captured desktop texture has an unsupported size or sample layout.");

            const bool sourceChanged = !m_sourceCopy ||
                m_sourceWidth != sourceDesc.Width ||
                m_sourceHeight != sourceDesc.Height ||
                m_sourceFormat != sourceDesc.Format;

            if (sourceChanged)
            {
                m_sourceCopy = nullptr;
                m_sourceSrv = nullptr;

                D3D11_TEXTURE2D_DESC copyDesc = sourceDesc;
                copyDesc.MipLevels = 1;
                copyDesc.ArraySize = 1;
                copyDesc.Usage = D3D11_USAGE_DEFAULT;
                copyDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                copyDesc.CPUAccessFlags = 0;
                copyDesc.MiscFlags = 0;
                copyDesc.SampleDesc.Count = 1;
                copyDesc.SampleDesc.Quality = 0;
                FOCUSGLASS_CHECK_HRESULT(m_device->CreateTexture2D(&copyDesc, nullptr, m_sourceCopy.put()));

                D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
                srvDesc.Format = sourceDesc.Format;
                srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                srvDesc.Texture2D.MostDetailedMip = 0;
                srvDesc.Texture2D.MipLevels = 1;
                FOCUSGLASS_CHECK_HRESULT(m_device->CreateShaderResourceView(m_sourceCopy.get(), &srvDesc, m_sourceSrv.put()));

                m_sourceWidth = sourceDesc.Width;
                m_sourceHeight = sourceDesc.Height;
                m_sourceFormat = sourceDesc.Format;

                const double scale = std::min(1.0, 640.0 / static_cast<double>(m_sourceWidth));
                m_blurWidth = std::max(1u, static_cast<UINT>(m_sourceWidth * scale));
                m_blurHeight = std::max(1u, static_cast<UINT>(m_sourceHeight * scale));

                m_downsampleTarget = {};
                m_horizontalTarget = {};
                m_blurTarget = {};
                CreateRenderTarget(m_blurWidth, m_blurHeight, m_downsampleTarget);
                CreateRenderTarget(m_blurWidth, m_blurHeight, m_horizontalTarget);
                CreateRenderTarget(m_blurWidth, m_blurHeight, m_blurTarget);
            }
        }

        void DrawPass(ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* source,
            ID3D11RenderTargetView* target, UINT width, UINT height,
            ID3D11Buffer* constants, bool useDownsampleShader)
        {
            ID3D11ShaderResourceView* nullView = nullptr;
            m_context->PSSetShaderResources(0, 1, &nullView);

            ID3D11RenderTargetView* targetView = target;
            m_context->OMSetRenderTargets(1, &targetView, nullptr);
            D3D11_VIEWPORT viewport{};
            viewport.Width = static_cast<float>(width);
            viewport.Height = static_cast<float>(height);
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;
            m_context->RSSetViewports(1, &viewport);
            m_context->IASetInputLayout(nullptr);
            m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_context->VSSetShader(m_vertexShader.get(), nullptr, 0);
            m_context->PSSetShader(useDownsampleShader ? m_downsamplePixelShader.get() : pixelShader, nullptr, 0);

            ID3D11ShaderResourceView* sourceView = source;
            m_context->PSSetShaderResources(0, 1, &sourceView);
            ID3D11SamplerState* sampler = m_sampler.get();
            m_context->PSSetSamplers(0, 1, &sampler);
            if (constants)
            {
                ID3D11Buffer* buffer = constants;
                m_context->PSSetConstantBuffers(0, 1, &buffer);
            }
            m_context->Draw(3, 0);

            m_context->PSSetShaderResources(0, 1, &nullView);
            m_context->OMSetRenderTargets(0, nullptr, nullptr);
        }

        winrt::com_ptr<ID3D11Device> m_device;
        winrt::com_ptr<ID3D11DeviceContext> m_context;
        int m_outputWidth{};
        int m_outputHeight{};

        winrt::com_ptr<ID3D11VertexShader> m_vertexShader;
        winrt::com_ptr<ID3D11PixelShader> m_downsamplePixelShader;
        winrt::com_ptr<ID3D11PixelShader> m_blurPixelShader;
        winrt::com_ptr<ID3D11PixelShader> m_compositePixelShader;
        winrt::com_ptr<ID3D11SamplerState> m_sampler;
        winrt::com_ptr<ID3D11Buffer> m_blurConstants;
        winrt::com_ptr<ID3D11Buffer> m_compositeConstants;

        winrt::com_ptr<IDXGISwapChain1> m_swapChain;
        winrt::com_ptr<IDXGISwapChain3> m_swapChain3;
        winrt::com_ptr<ID3D11RenderTargetView> m_swapChainTarget;
        winrt::com_ptr<IDCompositionDevice> m_compositionDevice;
        winrt::com_ptr<IDCompositionTarget> m_compositionTarget;
        winrt::com_ptr<IDCompositionVisual> m_visual;

        winrt::com_ptr<ID3D11Texture2D> m_sourceCopy;
        winrt::com_ptr<ID3D11ShaderResourceView> m_sourceSrv;
        UINT m_sourceWidth{};
        UINT m_sourceHeight{};
        DXGI_FORMAT m_sourceFormat{ DXGI_FORMAT_UNKNOWN };
        UINT m_blurWidth{};
        UINT m_blurHeight{};
        RenderTarget m_downsampleTarget;
        RenderTarget m_horizontalTarget;
        RenderTarget m_blurTarget;
    };
#undef FOCUSGLASS_CHECK_HRESULT
}


namespace
{
    namespace wgc = winrt::Windows::Graphics::Capture;
    namespace wdx = winrt::Windows::Graphics::DirectX;
    namespace wd3d = winrt::Windows::Graphics::DirectX::Direct3D11;

    // The Windhawk LLVM/MinGW SDK doesn't ship
    // windows.graphics.directx.direct3d11.interop.h. Keep the small COM ABI
    // declaration local, as used by the official Windhawk Alt-Tab Flip 3D mod.
    struct IDirect3DDxgiInterfaceAccess : public IUnknown
    {
        virtual HRESULT STDMETHODCALLTYPE GetInterface(REFIID iid, void** value) = 0;
    };

    constexpr GUID kIID_IDirect3DDxgiInterfaceAccess = {
        0xa9b3d012, 0x3df2, 0x4ee3,
        { 0xb8, 0xd1, 0x86, 0x95, 0xf4, 0x57, 0xd3, 0xc1 }
    };

    using CreateDirect3D11DeviceFromDXGIDevice_t =
        HRESULT(WINAPI*)(IDXGIDevice*, ::IInspectable**);

    constexpr wchar_t kAppClass[] = L"FocusGlass.HiddenHost";
    constexpr wchar_t kOverlayClass[] = L"FocusGlass.DesktopOverlay";
    constexpr UINT kTrayMessage = WM_APP + 10;
    constexpr UINT kNewFrameMessage = WM_APP + 11;
    constexpr UINT kRefreshMaskMessage = WM_APP + 12;
    constexpr UINT kErrorMessage = WM_APP + 13;
    constexpr UINT kSettingsChangedMessage = WM_APP + 14;
    constexpr UINT kHotkeyToggle = 1;
    constexpr UINT kHotkeyIncreaseBlur = 3;
    constexpr UINT kHotkeyDecreaseBlur = 4;
    constexpr UINT kMenuToggle = 1001;
    constexpr UINT kMenuIncreaseBlur = 1003;
    constexpr UINT kMenuDecreaseBlur = 1004;
    constexpr UINT kWdaExcludeFromCapture = 0x00000011;
    constexpr int kTopSafeAreaPx = 30;       // Keeps the user's macOS-style top bar clear.
    constexpr int kMinBlurLevel = 1;
    constexpr int kMaxBlurLevel = 5;
    constexpr int kInitialBlurLevel = 1;

    std::atomic<HWND> g_hostWindow{ nullptr };
    HWND g_overlayWindow = nullptr;
    std::atomic<bool> g_refreshMessagePending{ false };
    std::atomic<int> g_configuredBlurLevel{ kInitialBlurLevel };
    std::atomic<bool> g_configuredStartPaused{ false };
    std::atomic<bool> g_appStartSucceeded{ false };
    HANDLE g_appReadyEvent = nullptr;
    HANDLE g_appThread = nullptr;
    DWORD g_appThreadId = 0;

    void LoadWindhawkSettings()
    {
        const int configuredLevel = Wh_GetIntSetting(L"BlurLevel");
        g_configuredBlurLevel.store(
            std::clamp(configuredLevel, kMinBlurLevel, kMaxBlurLevel),
            std::memory_order_release);
        g_configuredStartPaused.store(
            Wh_GetIntSetting(L"StartPaused") != 0,
            std::memory_order_release);
    }

    void Log(const std::wstring& message)
    {
        Wh_Log(L"%s", message.c_str());
    }

    bool RegisterAppHotKey(HWND window, UINT id, UINT modifiers, UINT virtualKey,
        const wchar_t* description)
    {
        if (RegisterHotKey(window, id, modifiers, virtualKey))
            return true;

        const DWORD error = GetLastError();
        Log(std::wstring(L"Could not register ") + description +
            L" (Win32 error " + std::to_wstring(error) + L").");
        return false;
    }

    struct MonitorInfo
    {
        HMONITOR handle{};
        RECT bounds{};
        int width{};
        int height{};
    };

    MonitorInfo GetPrimaryMonitor()
    {
        MonitorInfo result{};
        result.handle = MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO info{ sizeof(info) };
        if (!result.handle || !GetMonitorInfoW(result.handle, &info))
            throw std::runtime_error("Could not read primary monitor information.");
        result.bounds = info.rcMonitor;
        result.width = result.bounds.right - result.bounds.left;
        result.height = result.bounds.bottom - result.bounds.top;
        if (result.width <= 0 || result.height <= 0)
            throw std::runtime_error("Primary monitor has an invalid size.");
        return result;
    }

    class CaptureEngine : public std::enable_shared_from_this<CaptureEngine>
    {
    public:
        struct FrameSnapshot
        {
            wgc::Direct3D11CaptureFrame frame{ nullptr };
            winrt::com_ptr<ID3D11Texture2D> texture;
            UINT width{};
            UINT height{};

            ~FrameSnapshot()
            {
                if (frame)
                {
                    try { frame.Close(); }
                    catch (...) { }
                }
            }
        };

        explicit CaptureEngine(HWND notifyWindow) : m_notifyWindow(notifyWindow) { }

        void Start(const MonitorInfo& monitor)
        {
            m_monitor = monitor;
            m_stopping.store(false);
            m_frameMessagePending.store(false);

            if (!wgc::GraphicsCaptureSession::IsSupported())
                throw std::runtime_error("Windows Graphics Capture is not available on this PC.");

            UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
            D3D_FEATURE_LEVEL levels[] = {
                D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0
            };
            D3D_FEATURE_LEVEL chosen{};
            HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                flags, levels, ARRAYSIZE(levels), D3D11_SDK_VERSION,
                m_d3dDevice.put(), &chosen, m_d3dContext.put());
            winrt::check_hresult(hr);

            winrt::com_ptr<IDXGIDevice> dxgiDevice;
            winrt::check_hresult(m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), dxgiDevice.put_void()));

            // Resolve the WinRT bridge dynamically instead of relying on the
            // omitted windows.graphics.directx.direct3d11.interop.h declaration.
            const HMODULE d3d11Module = GetModuleHandleW(L"d3d11.dll");
            const auto createCaptureDevice = d3d11Module
                ? reinterpret_cast<CreateDirect3D11DeviceFromDXGIDevice_t>(
                    GetProcAddress(d3d11Module, "CreateDirect3D11DeviceFromDXGIDevice"))
                : nullptr;
            if (!createCaptureDevice)
                throw std::runtime_error("CreateDirect3D11DeviceFromDXGIDevice is unavailable.");

            winrt::com_ptr<::IInspectable> inspectableDevice;
            winrt::check_hresult(createCaptureDevice(dxgiDevice.get(), inspectableDevice.put()));
            winrt::check_hresult(inspectableDevice->QueryInterface(
                winrt::guid_of<wd3d::IDirect3DDevice>(), winrt::put_abi(m_winrtDevice)));

            auto interop = winrt::get_activation_factory<wgc::GraphicsCaptureItem, ::IGraphicsCaptureItemInterop>();
            winrt::check_hresult(interop->CreateForMonitor(monitor.handle,
                winrt::guid_of<wgc::GraphicsCaptureItem>(), winrt::put_abi(m_item)));

            const auto size = m_item.Size();
            m_framePool = wgc::Direct3D11CaptureFramePool::CreateFreeThreaded(
                m_winrtDevice, wdx::DirectXPixelFormat::B8G8R8A8UIntNormalized, 2, size);
            m_session = m_framePool.CreateCaptureSession(m_item);
            m_session.IsCursorCaptureEnabled(false);
            if (auto session3 = m_session.try_as<wgc::IGraphicsCaptureSession3>())
                session3.IsBorderRequired(false);

            std::weak_ptr<CaptureEngine> weak = shared_from_this();
            m_frameToken = m_framePool.FrameArrived([weak](
                const wgc::Direct3D11CaptureFramePool& sender,
                const winrt::Windows::Foundation::IInspectable&)
            {
                if (auto self = weak.lock())
                    self->OnFrameArrived(sender);
            });

            m_session.StartCapture();
            Log(L"Windows Graphics Capture started; frames will stay on the GPU.");
        }

        void Stop() noexcept
        {
            m_stopping.store(true);
            try
            {
                if (m_framePool)
                    m_framePool.FrameArrived(m_frameToken);
                if (m_session) m_session.Close();
                if (m_framePool) m_framePool.Close();
            }
            catch (...) { }

            m_session = nullptr;
            m_framePool = nullptr;
            {
                std::lock_guard<std::mutex> lock(m_frameMutex);
                m_latestFrame.reset();
            }
            m_frameMessagePending.store(false);
            m_winrtDevice = nullptr;
            m_d3dContext = nullptr;
            m_d3dDevice = nullptr;
        }

        ID3D11Device* Device() const noexcept { return m_d3dDevice.get(); }
        ID3D11DeviceContext* Context() const noexcept { return m_d3dContext.get(); }

        bool GetLatestFrame(std::shared_ptr<FrameSnapshot>& frame)
        {
            std::lock_guard<std::mutex> lock(m_frameMutex);
            frame = m_latestFrame;
            return frame && frame->texture.get() != nullptr;
        }

        void ClearFrameMessagePending() noexcept
        {
            m_frameMessagePending.store(false, std::memory_order_release);
        }

    private:
        void OnFrameArrived(const wgc::Direct3D11CaptureFramePool& sender)
        {
            if (m_stopping.load(std::memory_order_acquire))
                return;

            try
            {
                auto capturedFrame = sender.TryGetNextFrame();
                if (!capturedFrame)
                    return;

                auto snapshot = std::make_shared<FrameSnapshot>();
                snapshot->frame = capturedFrame;

                auto surface = capturedFrame.Surface();
                winrt::com_ptr<IDirect3DDxgiInterfaceAccess> access;
                winrt::check_hresult(winrt::get_unknown(surface)->QueryInterface(
                    kIID_IDirect3DDxgiInterfaceAccess, access.put_void()));
                winrt::check_hresult(access->GetInterface(
                    __uuidof(ID3D11Texture2D), snapshot->texture.put_void()));

                D3D11_TEXTURE2D_DESC desc{};
                snapshot->texture->GetDesc(&desc);
                if (desc.Format != DXGI_FORMAT_B8G8R8A8_UNORM &&
                    desc.Format != DXGI_FORMAT_B8G8R8A8_UNORM_SRGB)
                    throw std::runtime_error("Unexpected Windows Graphics Capture pixel format.");

                snapshot->width = desc.Width;
                snapshot->height = desc.Height;

                // Keep the WGC frame alive until CopyResource has consumed its texture.
                {
                    std::lock_guard<std::mutex> lock(m_frameMutex);
                    m_latestFrame = std::move(snapshot);
                }

                // Coalesce frame notifications instead of queuing stale frames behind the UI.
                if (!m_frameMessagePending.exchange(true, std::memory_order_acq_rel))
                {
                    if (!PostMessageW(m_notifyWindow, kNewFrameMessage, 0, 0))
                        m_frameMessagePending.store(false, std::memory_order_release);
                }
            }
            catch (const winrt::hresult_error& e)
            {
                Log(std::wstring(L"Capture callback failed: ") + e.message().c_str());
                PostMessageW(m_notifyWindow, kErrorMessage, 0, 0);
            }
            catch (const std::exception& e)
            {
                Log(std::wstring(L"Capture callback failed: ") + winrt::to_hstring(e.what()).c_str());
                PostMessageW(m_notifyWindow, kErrorMessage, 0, 0);
            }
            catch (...)
            {
                Log(L"Capture callback failed with an unknown error.");
                PostMessageW(m_notifyWindow, kErrorMessage, 0, 0);
            }
        }

        HWND m_notifyWindow{};
        MonitorInfo m_monitor{};
        std::atomic<bool> m_stopping{ false };
        std::atomic<bool> m_frameMessagePending{ false };
        winrt::com_ptr<ID3D11Device> m_d3dDevice;
        winrt::com_ptr<ID3D11DeviceContext> m_d3dContext;
        wd3d::IDirect3DDevice m_winrtDevice{ nullptr };
        wgc::GraphicsCaptureItem m_item{ nullptr };
        wgc::Direct3D11CaptureFramePool m_framePool{ nullptr };
        wgc::GraphicsCaptureSession m_session{ nullptr };
        winrt::event_token m_frameToken{};
        std::mutex m_frameMutex;
        std::shared_ptr<FrameSnapshot> m_latestFrame;
    };


    bool GetVisibleFrameBounds(HWND hwnd, RECT& rect)
    {
        if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(rect))))
            return true;
        return GetWindowRect(hwnd, &rect) != FALSE;
    }

    bool IsVisibleOwnedPopup(HWND candidate, HWND foreground)
    {
        if (candidate == foreground || !IsWindowVisible(candidate) || IsIconic(candidate))
            return false;

        // Keep only popups owned by the foreground window (or one of its owned
        // popups) clear. Unrelated topmost/background windows stay behind the blur.
        HWND owner = GetWindow(candidate, GW_OWNER);
        for (int depth = 0; owner && depth < 16; ++depth)
        {
            if (owner == foreground) return true;
            owner = GetWindow(owner, GW_OWNER);
        }
        return false;
    }

    struct OwnedPopupContext
    {
        HWND foreground{};
        std::vector<RECT>* regions{};
    };

    BOOL CALLBACK EnumOwnedPopups(HWND hwnd, LPARAM parameter)
    {
        auto* context = reinterpret_cast<OwnedPopupContext*>(parameter);
        if (!IsVisibleOwnedPopup(hwnd, context->foreground))
            return TRUE;

        RECT rect{};
        if (GetVisibleFrameBounds(hwnd, rect))
            context->regions->push_back(rect);
        return TRUE;
    }


    bool RectanglesIntersect(const RECT& first, const RECT& second)
    {
        return first.left < second.right && first.right > second.left &&
            first.top < second.bottom && first.bottom > second.top;
    }

    bool RectanglesEqual(const RECT& first, const RECT& second)
    {
        return first.left == second.left && first.top == second.top &&
            first.right == second.right && first.bottom == second.bottom;
    }

    struct TopLevelWindowRecord
    {
        HWND hwnd{};
        RECT bounds{};
    };

    struct TopLevelWindowContext
    {
        HWND foreground{};
        std::vector<TopLevelWindowRecord>* windows{};
    };

    BOOL CALLBACK EnumTopLevelApplicationWindows(HWND hwnd, LPARAM parameter)
    {
        auto* context = reinterpret_cast<TopLevelWindowContext*>(parameter);
        if (!context || !context->windows || !IsWindowVisible(hwnd) || IsIconic(hwnd))
            return TRUE;

        if (hwnd == GetDesktopWindow() || hwnd == GetShellWindow() ||
            hwnd == g_hostWindow.load(std::memory_order_acquire) || hwnd == g_overlayWindow)
            return TRUE;

        // The focused window is always retained. Other candidates must be
        // independent application windows, not owned dialogs or utility tools.
        if (hwnd != context->foreground)
        {
            const LONG_PTR extendedStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
            if ((extendedStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) != 0 ||
                GetWindow(hwnd, GW_OWNER) != nullptr)
                return TRUE;
        }

        wchar_t className[128]{};
        GetClassNameW(hwnd, className, ARRAYSIZE(className));
        if (wcscmp(className, L"Progman") == 0 ||
            wcscmp(className, L"WorkerW") == 0 ||
            wcscmp(className, L"Shell_TrayWnd") == 0 ||
            wcscmp(className, L"Shell_SecondaryTrayWnd") == 0)
            return TRUE;

        DWORD cloaked = 0;
        if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
            cloaked != 0)
            return TRUE;

        RECT bounds{};
        if (GetVisibleFrameBounds(hwnd, bounds) &&
            bounds.right > bounds.left && bounds.bottom > bounds.top)
            context->windows->push_back({ hwnd, bounds });

        return TRUE;
    }

    struct WindowLayout
    {
        bool shouldBlur{};
        std::vector<RECT> clearRegions;
    };

    WindowLayout BuildWindowLayout(HWND foreground, const RECT& foregroundRect,
        const MonitorInfo& monitor)
    {
        WindowLayout layout;
        std::vector<TopLevelWindowRecord> windows;
        TopLevelWindowContext windowContext{ foreground, &windows };
        EnumWindows(EnumTopLevelApplicationWindows, reinterpret_cast<LPARAM>(&windowContext));

        // EnumWindows returns top-level windows in z-order (front to back).
        // Only windows below the active window can be its background.
        const auto active = std::find_if(windows.begin(), windows.end(),
            [foreground](const TopLevelWindowRecord& window) { return window.hwnd == foreground; });
        if (active == windows.end())
            return layout;

        const size_t activeIndex = static_cast<size_t>(std::distance(windows.begin(), active));
        bool hasOverlappingBackground = false;
        for (size_t index = activeIndex + 1; index < windows.size(); ++index)
        {
            if (RectanglesIntersect(foregroundRect, windows[index].bounds))
            {
                hasOverlappingBackground = true;
                break;
            }
        }

        // Side-by-side windows are intentional simultaneous workspaces.
        // With no lower-z-order app window under the foreground window, do not
        // create the blur overlay at all.
        if (!hasOverlappingBackground)
            return layout;

        layout.shouldBlur = true;
        layout.clearRegions.push_back(foregroundRect);

        // Preserve independent windows that sit beside the active window.
        // Only geometrically overlapping windows behind the active window
        // remain under the blurred backdrop.
        for (size_t index = activeIndex + 1; index < windows.size(); ++index)
        {
            if (!RectanglesIntersect(foregroundRect, windows[index].bounds))
                layout.clearRegions.push_back(windows[index].bounds);
        }

        // Keep popups owned by the foreground application sharp.
        OwnedPopupContext popupContext{ foreground, &layout.clearRegions };
        EnumWindows(EnumOwnedPopups, reinterpret_cast<LPARAM>(&popupContext));

        // Keep the Windows taskbar and secondary taskbars crisp.
        struct TaskbarEnum
        {
            std::vector<RECT>* regions;
            static BOOL CALLBACK Proc(HWND hwnd, LPARAM param)
            {
                wchar_t className[128]{};
                GetClassNameW(hwnd, className, ARRAYSIZE(className));
                if (wcscmp(className, L"Shell_TrayWnd") == 0 ||
                    wcscmp(className, L"Shell_SecondaryTrayWnd") == 0)
                {
                    RECT rect{};
                    if (GetWindowRect(hwnd, &rect))
                        reinterpret_cast<TaskbarEnum*>(param)->regions->push_back(rect);
                }
                return TRUE;
            }
        } taskbarContext{ &layout.clearRegions };
        EnumWindows(TaskbarEnum::Proc, reinterpret_cast<LPARAM>(&taskbarContext));

        // Preserve the user's custom macOS-style menu bar near the top edge.
        layout.clearRegions.push_back(RECT{ monitor.bounds.left, monitor.bounds.top,
            monitor.bounds.right, std::min(monitor.bounds.bottom, monitor.bounds.top + kTopSafeAreaPx) });
        return layout;
    }

    class App
    {
    public:
        ~App() { Shutdown(); }

        int Run(HINSTANCE instance, HANDLE readyEvent)
        {
            m_instance = instance;
            m_monitor = GetPrimaryMonitor();
            RegisterHostWindow();
            RegisterOverlayWindow();

            if (!SetWindowDisplayAffinity(m_overlay, kWdaExcludeFromCapture))
                throw std::runtime_error("Windows could not exclude the overlay from screen capture.");

            RegisterAppHotKey(m_host, kHotkeyToggle, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, 'B',
                L"Ctrl+Alt+B pause/resume hotkey");
            RegisterAppHotKey(m_host, kHotkeyIncreaseBlur, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_UP,
                L"Ctrl+Alt+Up blur-increase hotkey");
            RegisterAppHotKey(m_host, kHotkeyDecreaseBlur, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_DOWN,
                L"Ctrl+Alt+Down blur-decrease hotkey");

            m_foregroundHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
            m_locationHook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
            m_windowStateHook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_REORDER,
                nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
            m_layoutCacheEnabled = m_foregroundHook && m_locationHook && m_windowStateHook;
            if (!m_layoutCacheEnabled)
                Log(L"One or more window event hooks could not be installed; using per-frame window detection.");

            AddTrayIcon();
            if (g_configuredStartPaused.load(std::memory_order_acquire))
            {
                UpdateTrayTip(L"FocusGlass - paused (Ctrl+Alt+B to resume)");
            }
            else
            {
                Toggle();
                if (!m_enabled)
                    throw std::runtime_error("FocusGlass could not initialize its capture or GPU compositor.");
            }

            Log(L"FocusGlass Windhawk Tool Mod started. GPU Gaussian blur and DirectComposition are active.");
            g_appStartSucceeded.store(true, std::memory_order_release);
            if (readyEvent)
                SetEvent(readyEvent);

            MSG message{};
            BOOL result = 0;
            while ((result = GetMessageW(&message, nullptr, 0, 0)) > 0)
            {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
            if (result == -1)
                Log(L"GetMessage failed in the FocusGlass message loop.");

            Shutdown();
            return result == -1 ? 1 : static_cast<int>(message.wParam);
        }

    private:
        static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND,
            LONG idObject, LONG idChild, DWORD, DWORD)
        {
            const HWND hostWindow = g_hostWindow.load(std::memory_order_acquire);
            if (!hostWindow)
                return;

            const bool foregroundChanged = event == EVENT_SYSTEM_FOREGROUND;
            const bool topLevelWindowChanged =
                (event == EVENT_OBJECT_LOCATIONCHANGE ||
                    (event >= EVENT_OBJECT_CREATE && event <= EVENT_OBJECT_REORDER)) &&
                idObject == OBJID_WINDOW && idChild == CHILDID_SELF;

            if (!foregroundChanged && !topLevelWindowChanged)
                return;

            // Window moves can generate many accessibility events. Coalesce them
            // into one UI-thread refresh instead of growing the message queue.
            if (!g_refreshMessagePending.exchange(true, std::memory_order_acq_rel))
            {
                if (!PostMessageW(hostWindow, kRefreshMaskMessage, 0, 0))
                    g_refreshMessagePending.store(false, std::memory_order_release);
            }
        }

        static LRESULT CALLBACK HostWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            App* self = reinterpret_cast<App*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            if (message == WM_NCCREATE)
            {
                auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
                self = static_cast<App*>(create->lpCreateParams);
                SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
            }
            if (!self) return DefWindowProcW(hwnd, message, wParam, lParam);

            switch (message)
            {
            case WM_HOTKEY:
                if (wParam == kHotkeyToggle) self->Toggle();
                else if (wParam == kHotkeyIncreaseBlur) self->AdjustBlur(+1);
                else if (wParam == kHotkeyDecreaseBlur) self->AdjustBlur(-1);
                return 0;
            case WM_COMMAND:
                if (LOWORD(wParam) == kMenuToggle) self->Toggle();
                else if (LOWORD(wParam) == kMenuIncreaseBlur) self->AdjustBlur(+1);
                else if (LOWORD(wParam) == kMenuDecreaseBlur) self->AdjustBlur(-1);
                return 0;
            case kTrayMessage:
                if (lParam == WM_RBUTTONUP || lParam == WM_CONTEXTMENU) self->ShowTrayMenu();
                else if (lParam == WM_LBUTTONUP) self->Toggle();
                return 0;
            case kNewFrameMessage:
                if (self->m_capture) self->m_capture->ClearFrameMessagePending();
                self->RenderLatest();
                return 0;
            case kRefreshMaskMessage:
                g_refreshMessagePending.store(false, std::memory_order_release);
                self->m_layoutDirty = true;
                self->RenderLatest();
                return 0;
            case kSettingsChangedMessage:
                self->m_blurLevel->store(
                    g_configuredBlurLevel.load(std::memory_order_acquire),
                    std::memory_order_relaxed);
                self->m_layoutDirty = true;
                if ((!g_configuredStartPaused.load(std::memory_order_acquire)) != self->m_enabled)
                    self->Toggle();
                return 0;
            case kErrorMessage:
                self->HideOverlay();
                self->m_enabled = false;
                self->m_compositor.reset();
                if (self->m_capture) self->m_capture->Stop();
                self->m_capture.reset();
                self->UpdateTrayTip(L"FocusGlass - stopped after capture error");
                return 0;
            case WM_DISPLAYCHANGE:
                // The capture surface is tied to the original monitor topology.
                // Invalidate cached geometry and require a restart for a safe reinitialization.
                self->m_layoutDirty = true;
                self->m_cachedForegroundWindow = nullptr;
                self->HideOverlay();
                self->m_enabled = false;
                self->m_compositor.reset();
                if (self->m_capture) self->m_capture->Stop();
                self->m_capture.reset();
                Log(L"Display configuration changed. Restart FocusGlass to reinitialize capture safely.");
                self->UpdateTrayTip(L"FocusGlass - restart required after display change");
                return 0;
            case WM_CLOSE:
                DestroyWindow(hwnd);
                return 0;
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            }
            return DefWindowProcW(hwnd, message, wParam, lParam);
        }

        static LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            if (message == WM_NCHITTEST)
                return HTTRANSPARENT;
            return DefWindowProcW(hwnd, message, wParam, lParam);
        }

        void RegisterHostWindow()
        {
            WNDCLASSEXW wc{ sizeof(wc) };
            wc.lpfnWndProc = HostWndProc;
            wc.hInstance = m_instance;
            wc.lpszClassName = kAppClass;
            wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
            RegisterClassExW(&wc);
            m_host = CreateWindowExW(0, kAppClass, L"FocusGlass host", WS_OVERLAPPED,
                0, 0, 0, 0, nullptr, nullptr, m_instance, this);
            if (!m_host) throw std::runtime_error("Could not create FocusGlass host window.");
            g_hostWindow.store(m_host, std::memory_order_release);
        }

        void RegisterOverlayWindow()
        {
            WNDCLASSEXW wc{ sizeof(wc) };
            wc.lpfnWndProc = OverlayWndProc;
            wc.hInstance = m_instance;
            wc.lpszClassName = kOverlayClass;
            RegisterClassExW(&wc);

            // DirectComposition owns the visual surface; the HWND has no separate layered bitmap.
            // DirectComposition supports layered HWND targets. Layered + transparent
            // provides reliable click-through to background applications.
            const DWORD exStyle = WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT |
                WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
            m_overlay = CreateWindowExW(exStyle, kOverlayClass, L"FocusGlass GPU backdrop", WS_POPUP,
                m_monitor.bounds.left, m_monitor.bounds.top, m_monitor.width, m_monitor.height,
                nullptr, nullptr, m_instance, nullptr);
            if (!m_overlay)
                throw std::runtime_error("Could not create the DirectComposition overlay window.");
            if (!SetLayeredWindowAttributes(m_overlay, 0, 255, LWA_ALPHA))
                throw std::runtime_error("Could not initialize the layered DirectComposition overlay.");
            g_overlayWindow = m_overlay;
        }

        void AddTrayIcon()
        {
            m_tray = {};
            m_tray.cbSize = sizeof(m_tray);
            m_tray.hWnd = m_host;
            m_tray.uID = 1;
            m_tray.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
            m_tray.uCallbackMessage = kTrayMessage;
            m_tray.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
            wcscpy_s(m_tray.szTip, L"FocusGlass - active (Ctrl+Alt+B to pause)");
            Shell_NotifyIconW(NIM_ADD, &m_tray);
        }

        void UpdateTrayTip(const wchar_t* tip)
        {
            if (!m_tray.hWnd) return;
            m_tray.uFlags = NIF_TIP;
            wcsncpy_s(m_tray.szTip, tip, _TRUNCATE);
            Shell_NotifyIconW(NIM_MODIFY, &m_tray);
        }

        void ShowTrayMenu()
        {
            POINT point{};
            GetCursorPos(&point);
            HMENU menu = CreatePopupMenu();
            if (!menu) return;
            const int level = m_blurLevel->load(std::memory_order_relaxed);
            const UINT increaseFlags = MF_STRING | (level >= kMaxBlurLevel ? MF_GRAYED : 0);
            const UINT decreaseFlags = MF_STRING | (level <= kMinBlurLevel ? MF_GRAYED : 0);
            AppendMenuW(menu, increaseFlags, kMenuIncreaseBlur, L"Increase blur  (Ctrl+Alt+Up)");
            AppendMenuW(menu, decreaseFlags, kMenuDecreaseBlur, L"Decrease blur  (Ctrl+Alt+Down)");
            AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(menu, MF_STRING, kMenuToggle, m_enabled ? L"Pause focus blur" : L"Resume focus blur");
            HideOverlay();
            SetForegroundWindow(m_host);
            TrackPopupMenu(menu, TPM_RIGHTBUTTON | TPM_BOTTOMALIGN, point.x, point.y, 0, m_host, nullptr);
            DestroyMenu(menu);
            if (m_enabled) ShowOverlay();
        }

        void AdjustBlur(int delta)
        {
            const int current = m_blurLevel->load(std::memory_order_relaxed);
            const int next = std::clamp(current + delta, kMinBlurLevel, kMaxBlurLevel);
            if (next == current)
            {
                ShowBlurNotification(current >= kMaxBlurLevel
                    ? L"Maximum blur strength reached."
                    : L"Minimum blur strength reached.");
                return;
            }

            m_blurLevel->store(next, std::memory_order_relaxed);
            const std::wstring status = L"FocusGlass - " + std::to_wstring(next) +
                L"/5 blur (Ctrl+Alt+Up/Down)";
            UpdateTrayTip(status.c_str());

            const std::wstring message = L"Blur strength: " + std::to_wstring(next) + L"/5";
            ShowBlurNotification(message.c_str());
            g_configuredBlurLevel.store(next, std::memory_order_release);
            Log(L"Blur strength changed to " + std::to_wstring(next) + L"/5.");
        }

        void ShowBlurNotification(const wchar_t* message)
        {
            if (!m_tray.hWnd) return;
            NOTIFYICONDATAW notification = m_tray;
            notification.uFlags = NIF_INFO;
            wcscpy_s(notification.szInfoTitle, L"FocusGlass");
            wcsncpy_s(notification.szInfo, message, _TRUNCATE);
            notification.dwInfoFlags = NIIF_INFO;
            notification.uTimeout = 1500;
            Shell_NotifyIconW(NIM_MODIFY, &notification);
        }

        void Toggle()
        {
            if (m_enabled)
            {
                m_enabled = false;
                HideOverlay();
                m_compositor.reset();
                if (m_capture) m_capture->Stop();
                m_capture.reset();
                UpdateTrayTip(L"FocusGlass - paused (Ctrl+Alt+B to resume)");
                return;
            }

            try
            {
                m_layoutDirty = true;
                m_cachedForegroundWindow = nullptr;
                m_capture = std::make_shared<CaptureEngine>(m_host);
                m_capture->Start(m_monitor);
                m_compositor = std::make_unique<focusglass::GpuCompositor>(
                    m_capture->Device(), m_capture->Context(), m_overlay,
                    m_monitor.width, m_monitor.height);
                m_enabled = true;
                ShowOverlay();
                UpdateTrayTip(L"FocusGlass - GPU blur active (Ctrl+Alt+B to pause)");
            }
            catch (const std::exception& e)
            {
                m_compositor.reset();
                if (m_capture) m_capture->Stop();
                m_capture.reset();
                m_enabled = false;
                HideOverlay();
                Log(std::wstring(L"Could not resume GPU blur: ") + winrt::to_hstring(e.what()).c_str());
                UpdateTrayTip(L"FocusGlass - could not resume; see log");
            }
            catch (...)
            {
                m_compositor.reset();
                if (m_capture) m_capture->Stop();
                m_capture.reset();
                m_enabled = false;
                HideOverlay();
                Log(L"Could not resume GPU blur due to an unknown error.");
                UpdateTrayTip(L"FocusGlass - could not resume; see log");
            }
        }

        void ShowOverlay()
        {
            if (!m_overlay) return;
            ShowWindow(m_overlay, SW_SHOWNOACTIVATE);
            SetWindowPos(m_overlay, HWND_TOPMOST, m_monitor.bounds.left, m_monitor.bounds.top,
                m_monitor.width, m_monitor.height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
            RenderLatest();
        }

        void HideOverlay()
        {
            if (m_overlay) ShowWindow(m_overlay, SW_HIDE);
        }

        void RenderLatest()
        {
            if (!m_enabled || !m_capture || !m_compositor || !m_overlay)
                return;

            std::shared_ptr<CaptureEngine::FrameSnapshot> frame;
            if (!m_capture->GetLatestFrame(frame) || !frame || frame->texture.get() == nullptr)
                return;

            HWND foreground = GetForegroundWindow();
            if (!foreground || foreground == m_host || foreground == m_overlay ||
                foreground == GetShellWindow())
            {
                HideOverlay();
                return;
            }

            RECT foregroundRect{};
            if (!GetVisibleFrameBounds(foreground, foregroundRect) ||
                foregroundRect.right <= m_monitor.bounds.left || foregroundRect.left >= m_monitor.bounds.right ||
                foregroundRect.bottom <= m_monitor.bounds.top || foregroundRect.top >= m_monitor.bounds.bottom)
            {
                // Never apply this primary-monitor overlay when focus is elsewhere.
                m_layoutDirty = true;
                m_cachedForegroundWindow = nullptr;
                HideOverlay();
                return;
            }

            // Window geometry and z-order change far less often than captured
            // desktop frames. Rebuild the layout only when a relevant WinEvent
            // arrives, foreground changes, or the active frame bounds differ.
            if (!m_layoutCacheEnabled || m_layoutDirty ||
                m_cachedForegroundWindow != foreground ||
                !RectanglesEqual(m_cachedForegroundRect, foregroundRect))
            {
                m_cachedLayout = BuildWindowLayout(foreground, foregroundRect, m_monitor);
                m_cachedForegroundWindow = foreground;
                m_cachedForegroundRect = foregroundRect;
                m_layoutDirty = false;
            }

            const WindowLayout& layout = m_cachedLayout;
            if (!layout.shouldBlur)
            {
                HideOverlay();
                return;
            }

            try
            {
                // The retained WGC frame keeps the source texture valid until the GPU copy completes.
                m_compositor->Render(frame->texture.get(), m_monitor.bounds, layout.clearRegions,
                    m_blurLevel->load(std::memory_order_relaxed));
                ShowWindow(m_overlay, SW_SHOWNOACTIVATE);
            }
            catch (const winrt::hresult_error& e)
            {
                Log(std::wstring(L"GPU compositor failed: ") + e.message().c_str());
                HideOverlay();
                m_enabled = false;
                m_compositor.reset();
                if (m_capture) m_capture->Stop();
                m_capture.reset();
                UpdateTrayTip(L"FocusGlass - stopped after GPU error; see log");
            }
            catch (const std::exception& e)
            {
                Log(std::wstring(L"GPU compositor failed: ") + winrt::to_hstring(e.what()).c_str());
                HideOverlay();
                m_enabled = false;
                m_compositor.reset();
                if (m_capture) m_capture->Stop();
                m_capture.reset();
                UpdateTrayTip(L"FocusGlass - stopped after GPU error; see log");
            }
        }

        void Shutdown() noexcept
        {
            if (m_shutdown) return;
            m_shutdown = true;
            m_compositor.reset();
            if (m_capture) m_capture->Stop();
            m_capture.reset();
            HideOverlay();
            if (m_foregroundHook) UnhookWinEvent(m_foregroundHook);
            if (m_locationHook) UnhookWinEvent(m_locationHook);
            if (m_windowStateHook) UnhookWinEvent(m_windowStateHook);
            g_refreshMessagePending.store(false, std::memory_order_release);
            UnregisterHotKey(m_host, kHotkeyToggle);
            UnregisterHotKey(m_host, kHotkeyIncreaseBlur);
            UnregisterHotKey(m_host, kHotkeyDecreaseBlur);
            if (m_tray.hWnd) Shell_NotifyIconW(NIM_DELETE, &m_tray);
            if (m_overlay) DestroyWindow(m_overlay);
            if (m_host) DestroyWindow(m_host);
            g_overlayWindow = nullptr;
            g_hostWindow.store(nullptr, std::memory_order_release);
            Log(L"FocusGlass shut down cleanly.");
        }

        HINSTANCE m_instance{};
        HWND m_host{};
        HWND m_overlay{};
        MonitorInfo m_monitor{};
        HWINEVENTHOOK m_foregroundHook{};
        HWINEVENTHOOK m_locationHook{};
        HWINEVENTHOOK m_windowStateHook{};
        NOTIFYICONDATAW m_tray{};
        std::unique_ptr<focusglass::GpuCompositor> m_compositor;
        WindowLayout m_cachedLayout{};
        HWND m_cachedForegroundWindow{};
        RECT m_cachedForegroundRect{};
        bool m_layoutCacheEnabled{ false };
        bool m_layoutDirty{ true };
        bool m_enabled{ false };
        bool m_shutdown{ false };
        std::shared_ptr<std::atomic<int>> m_blurLevel{
            std::make_shared<std::atomic<int>>(g_configuredBlurLevel.load(std::memory_order_acquire)) };
        std::shared_ptr<CaptureEngine> m_capture;
    };
}


void FocusGlassDebugTrace(const std::wstring& message);

namespace
{
    DWORD WINAPI FocusGlassWorkerThread(LPVOID)
    {
        FocusGlassDebugTrace(L"Dedicated worker thread entered.");
        bool apartmentInitialized = false;
        try
        {
            MSG placeholder{};
            PeekMessageW(&placeholder, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

            SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            apartmentInitialized = true;
            FocusGlassDebugTrace(L"WinRT MTA apartment initialized.");

            App app;
            FocusGlassDebugTrace(L"Starting desktop capture and GPU compositor.");
            app.Run(GetModuleHandleW(nullptr), g_appReadyEvent);
            FocusGlassDebugTrace(L"App.Run returned.");
        }
        catch (const winrt::hresult_error& error)
        {
            FocusGlassDebugTrace(std::wstring(L"Startup HRESULT exception: ") + error.message().c_str());
            Log(std::wstring(L"FocusGlass startup failed: ") + error.message().c_str());
            if (g_appReadyEvent)
                SetEvent(g_appReadyEvent);
        }
        catch (const std::exception& error)
        {
            FocusGlassDebugTrace(std::wstring(L"Startup exception: ") + winrt::to_hstring(error.what()).c_str());
            Log(std::wstring(L"FocusGlass startup failed: ") + winrt::to_hstring(error.what()).c_str());
            if (g_appReadyEvent)
                SetEvent(g_appReadyEvent);
        }
        catch (...)
        {
            FocusGlassDebugTrace(L"Startup failed with an unknown exception.");
            Log(L"FocusGlass startup failed with an unknown error.");
            if (g_appReadyEvent)
                SetEvent(g_appReadyEvent);
        }

        if (apartmentInitialized)
            winrt::uninit_apartment();
        return 0;
    }

    void CloseWorkerHandles()
    {
        if (g_appThread)
        {
            CloseHandle(g_appThread);
            g_appThread = nullptr;
        }
        if (g_appReadyEvent)
        {
            CloseHandle(g_appReadyEvent);
            g_appReadyEvent = nullptr;
        }
        g_appThreadId = 0;
    }
}

BOOL WhTool_ModInit()
{
    FocusGlassDebugTrace(L"WhTool_ModInit entered in dedicated process.");
    LoadWindhawkSettings();
    g_appStartSucceeded.store(false, std::memory_order_release);
    g_appReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_appReadyEvent)
    {
        FocusGlassDebugTrace(L"CreateEvent failed with Win32 error " + std::to_wstring(GetLastError()) + L".");
        Wh_Log(L"FocusGlass: CreateEvent failed with error %lu", GetLastError());
        return FALSE;
    }

    g_appThread = CreateThread(nullptr, 0, FocusGlassWorkerThread, nullptr, 0, &g_appThreadId);
    if (!g_appThread)
    {
        FocusGlassDebugTrace(L"CreateThread for renderer failed.");
        Wh_Log(L"FocusGlass: CreateThread failed with error %lu", GetLastError());
        CloseWorkerHandles();
        return FALSE;
    }

    FocusGlassDebugTrace(L"Waiting up to 20 seconds for renderer initialization.");
    const DWORD waitResult = WaitForSingleObject(g_appReadyEvent, 20000);
    FocusGlassDebugTrace(L"Renderer startup wait finished: waitResult=" +
        std::to_wstring(waitResult) + L", success=" +
        std::to_wstring(g_appStartSucceeded.load(std::memory_order_acquire) ? 1 : 0) + L".");
    if (waitResult != WAIT_OBJECT_0 ||
        !g_appStartSucceeded.load(std::memory_order_acquire))
    {
        FocusGlassDebugTrace(L"Renderer startup did not complete successfully; stopping worker.");
        Wh_Log(L"FocusGlass: the renderer did not report a successful startup.");
        const HWND hostWindow = g_hostWindow.load(std::memory_order_acquire);
        if (hostWindow)
            PostMessageW(hostWindow, WM_CLOSE, 0, 0);
        else
            PostThreadMessageW(g_appThreadId, WM_QUIT, 0, 0);

        WaitForSingleObject(g_appThread, 10000);
        CloseWorkerHandles();
        return FALSE;
    }

    FocusGlassDebugTrace(L"WhTool_ModInit completed successfully; renderer is ready.");
    return TRUE;
}

void WhTool_ModSettingsChanged()
{
    LoadWindhawkSettings();
    const HWND hostWindow = g_hostWindow.load(std::memory_order_acquire);
    if (hostWindow)
        PostMessageW(hostWindow, kSettingsChangedMessage, 0, 0);
}

void WhTool_ModUninit()
{
    const HWND hostWindow = g_hostWindow.load(std::memory_order_acquire);
    if (hostWindow)
        PostMessageW(hostWindow, WM_CLOSE, 0, 0);
    else if (g_appThreadId)
        PostThreadMessageW(g_appThreadId, WM_QUIT, 0, 0);

    if (g_appThread)
    {
        DWORD waitResult = WaitForSingleObject(g_appThread, 10000);
        if (waitResult == WAIT_TIMEOUT)
        {
            Wh_Log(L"FocusGlass: graceful shutdown timed out; requesting message-loop exit.");
            if (g_appThreadId)
                PostThreadMessageW(g_appThreadId, WM_QUIT, 0, 0);
            waitResult = WaitForSingleObject(g_appThread, 5000);
        }
        if (waitResult == WAIT_TIMEOUT)
            Wh_Log(L"FocusGlass: worker thread did not stop before the Windhawk host exits.");
    }

    CloseWorkerHandles();
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk 1.7.3 Tool Mod launcher implementation.
// Keep this compatibility block for stable Windhawk 1.7.3. Windhawk 2.0 has
// native Tool Mod support, but its header isn't available to the 1.7.3 compiler.

bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

void FocusGlassDebugTrace(const std::wstring& message)
{
    Wh_Log(L"%s", message.c_str());
}

void WINAPI EntryPoint_Hook()
{
    Wh_Log(L"FocusGlass Tool Mod worker entry point");
    ExitThread(0);
}

BOOL Wh_ModInit()
{
    FocusGlassDebugTrace(L"Wh_ModInit entered. Command line: " + std::wstring(GetCommandLineW()));
    DWORD sessionId = 0;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0)
    {
        FocusGlassDebugTrace(L"Skipping Session 0 process.");
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv)
    {
        FocusGlassDebugTrace(L"CommandLineToArgvW failed.");
        Wh_Log(L"FocusGlass: CommandLineToArgvW failed.");
        return FALSE;
    }

    for (int i = 1; i < argc; ++i)
    {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0)
        {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; ++i)
    {
        if (wcscmp(argv[i], L"-tool-mod") == 0)
        {
            isToolModProcess = true;
            isCurrentToolModProcess = wcscmp(argv[i + 1], WH_MOD_ID) == 0;
            break;
        }
    }

    LocalFree(argv);
    FocusGlassDebugTrace(L"Process classification: excluded=" +
        std::to_wstring(isExcluded ? 1 : 0) + L", toolMod=" +
        std::to_wstring(isToolModProcess ? 1 : 0) + L", focusglassWorker=" +
        std::to_wstring(isCurrentToolModProcess ? 1 : 0) + L".");
    if (isExcluded)
    {
        FocusGlassDebugTrace(L"Skipping Windhawk service process.");
        return FALSE;
    }

    if (isCurrentToolModProcess)
    {
        g_toolModProcessMutex = CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex)
        {
            FocusGlassDebugTrace(L"Worker failed to create its single-instance mutex.");
            Wh_Log(L"FocusGlass: CreateMutex failed.");
            ExitProcess(1);
        }
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            FocusGlassDebugTrace(L"A FocusGlass Tool Mod worker already exists.");
            Wh_Log(L"FocusGlass Tool Mod process is already running.");
            ExitProcess(1);
        }
        if (!WhTool_ModInit())
        {
            FocusGlassDebugTrace(L"WhTool_ModInit returned FALSE; worker is exiting.");
            ExitProcess(1);
        }

        auto* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandleW(nullptr));
        auto* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(
            reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew);
        const DWORD entryPointRva = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = reinterpret_cast<BYTE*>(dosHeader) + entryPointRva;
        Wh_SetFunctionHook(entryPoint, reinterpret_cast<void*>(EntryPoint_Hook), nullptr);
        return TRUE;
    }

    if (isToolModProcess)
        return FALSE;

    g_isToolModProcessLauncher = true;
    FocusGlassDebugTrace(L"Launcher instance initialized; waiting for Wh_ModAfterInit.");
    return TRUE;
}

void Wh_ModAfterInit()
{
    if (!g_isToolModProcessLauncher)
    {
        FocusGlassDebugTrace(L"Wh_ModAfterInit called in worker/non-launcher process; no spawn required.");
        return;
    }
    FocusGlassDebugTrace(L"Wh_ModAfterInit entered; attempting to launch dedicated worker.");

    WCHAR currentProcessPath[MAX_PATH]{};
    const DWORD pathLength = GetModuleFileNameW(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath));
    if (pathLength == 0 || pathLength >= ARRAYSIZE(currentProcessPath))
    {
        FocusGlassDebugTrace(L"GetModuleFileName failed in launcher.");
        Wh_Log(L"FocusGlass: GetModuleFileName failed.");
        return;
    }

    WCHAR commandLine[MAX_PATH + 128]{};
    if (swprintf_s(commandLine, ARRAYSIZE(commandLine), L"\"%s\" -tool-mod \"%s\"",
        currentProcessPath, WH_MOD_ID) < 0)
    {
        FocusGlassDebugTrace(L"Failed to format the dedicated worker command line.");
        Wh_Log(L"FocusGlass: failed to format the Tool Mod command line.");
        return;
    }

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule)
        kernelModule = GetModuleHandleW(L"kernel32.dll");
    if (!kernelModule)
    {
        FocusGlassDebugTrace(L"Could not find kernelbase.dll or kernel32.dll.");
        Wh_Log(L"FocusGlass: kernelbase/kernel32 module was not found.");
        return;
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
        WINBOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
    auto createProcessInternal = reinterpret_cast<CreateProcessInternalW_t>(
        GetProcAddress(kernelModule, "CreateProcessInternalW"));
    if (!createProcessInternal)
    {
        FocusGlassDebugTrace(L"CreateProcessInternalW export was not found.");
        Wh_Log(L"FocusGlass: CreateProcessInternalW was not found.");
        return;
    }

    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION processInfo{};
    if (!createProcessInternal(nullptr, currentProcessPath, commandLine, nullptr, nullptr,
        FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &startupInfo, &processInfo, nullptr))
    {
        const DWORD error = GetLastError();
        FocusGlassDebugTrace(L"CreateProcessInternalW failed with Win32 error " +
            std::to_wstring(error) + L".");
        Wh_Log(L"FocusGlass: failed to launch dedicated Tool Mod process (error %lu).", error);
        return;
    }

    FocusGlassDebugTrace(L"Dedicated worker process created. PID=" +
        std::to_wstring(processInfo.dwProcessId) + L". Command line: " + commandLine);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
}

void Wh_ModSettingsChanged()
{
    if (!g_isToolModProcessLauncher)
        WhTool_ModSettingsChanged();
}

void Wh_ModUninit()
{
    if (g_isToolModProcessLauncher)
        return;

    WhTool_ModUninit();
    ExitProcess(0);
}