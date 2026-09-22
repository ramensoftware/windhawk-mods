"""Build the experiment; check geometry, isolated HWND transactions and cleanup.

Never loads it into Explorer. XAML ownership and animations require live checks.
"""
from pathlib import Path
import subprocess
import re

folder = Path(__file__).resolve().parent
production = folder.parents[2] / 'mods/windows-11-better-ultrawide-taskbar.wh.cpp'
source = folder / 'windows-11-better-ultrawide-taskbar.wh.cpp'
source.write_bytes(production.read_bytes())
code = source.read_text(encoding='utf-8')
version = re.search(r'@version\s+(\S+)', code)[1]


def function_body(signature):
    start = code.index(signature)
    opening = code.index('{', start)
    depth = 1
    for index in range(opening + 1, len(code)):
        depth += (code[index] == '{') - (code[index] == '}')
        if not depth:
            return code[opening + 1:index]
    raise AssertionError(f'unclosed function: {signature}')


init_body = function_body('BOOL Wh_ModInit() {')
late_load_body = function_body('HMODULE WINAPI LoadLibraryExW_Hook(')
after_init_body = function_body('void Wh_ModAfterInit() {')
hook_taskbar_body = function_body('bool TryHookTaskbarView(')
assert 'LoadLibraryExW_Hook' in init_body
assert 'TryHookTaskbarView' in late_load_body
assert 'TryHookTaskbarView' in after_init_body
assert 'HookTaskbarViewSymbols' in hook_taskbar_body
assert 'Wh_ApplyHookOperations' in hook_taskbar_body
print('PASS: Explorer startup waits for a late Taskbar.View.dll and applies hooks.')

windhawk = Path("C:/Applications/Windhawk")
compiler = windhawk / "Compiler"
flags = (compiler / "compile_flags.txt").read_text().splitlines()
flags.remove("-DWH_EDITING")
flags += ['-DWH_MOD_ID=L"windows-11-better-ultrawide-taskbar"',
          f'-DWH_MOD_VERSION=L"{version}"']
driver = ["rtk", "proxy", str(compiler / "bin/clang++.exe"), *flags]
libs = ["-lole32", "-loleaut32", "-lruntimeobject", "-ldwmapi",
        "-lwindowsapp", "-ld3d11", "-ldxgi", "-ld3dcompiler", "-lgdi32",
        "-luser32", "-lkernel32"]
source = folder / "windows-11-better-ultrawide-taskbar.wh.cpp"
subprocess.run([*driver, "-shared", "-O2", "-Wall", "-Wextra", "-Werror", str(source),
                "-x", "none", str(windhawk / "Engine/1.7.3/64/windhawk.lib"), *libs,
                "-o", str(folder / "candidate.build-check.dll")], check=True)
harness = folder / "candidate.check.cpp"
harness.write_text('''#include <windows.h>
#include <cassert>
#include <dispatcherqueue.h>
// Class/process filtering stays in the exercised production path. Only the
// image name of this test process is substituted; real Shell windows are unused.
BOOL WINAPI TestProcessName(HANDLE process, DWORD flags, LPWSTR path, PDWORD length) {
    if (GetProcessId(process) != GetCurrentProcessId())
        return QueryFullProcessImageNameW(process, flags, path, length);
    const wchar_t name[] = L"C:/test/ShellHost.exe";
    if (*length < ARRAYSIZE(name)) return FALSE;
    wcscpy_s(path, *length, name);
    // Use Windows separators without Python/C++ escape ambiguity.
    for (auto p = path; *p; ++p) if (*p == L'/') *p = wchar_t(92);
    *length = ARRAYSIZE(name) - 1;
    return TRUE;
}
// A live enabled copy can publish on real Explorer taskbars. Hide those
// properties from this harness so its fake taskbar is the sole test publisher.
HANDLE WINAPI TestGetProp(HWND window, LPCWSTR name) {
    DWORD pid{};
    GetWindowThreadProcessId(window, &pid);
    return pid == GetCurrentProcessId() ? GetPropW(window, name) : nullptr;
}
#define QueryFullProcessImageNameW TestProcessName
#define GetPropW TestGetProp
#define UWPANEL_PENDING_PROPERTY L"Windhawk.UWPOS09.PanelPending.Check"
#include "windows-11-better-ultrawide-taskbar.wh.cpp"
#undef UWPANEL_PENDING_PROPERTY
#undef GetPropW
#undef QueryFullProcessImageNameW
#include <cassert>
#include <limits>
#include <winrt/Windows.System.h>
int calls;
int owner;
int positionCalls;
UINT forwardedFlags;
HWND testBar;
HWND testSecondary;
bool withdrawDuringPosition;
bool failPosition;
int dwmCalls;
int hostCalls;
int secondaryHostCalls;
int hostReleases;
DWORD testUiThread;
void* testSite;
void* testSecondarySite;
int testControl;
void WINAPI TestDecref(void* control) {
    assert(control == &testControl);
    ++hostReleases;
}
void* WINAPI TestGetHost(void* site, void** host) {
    assert(site == testSite && GetCurrentThreadId() == testUiThread);
    ++hostCalls;
    host[0] = nullptr;
    host[1] = &testControl;
    return host;
}
void* WINAPI TestGetSecondaryHost(void* site, void** host) {
    assert(site == testSecondarySite && GetCurrentThreadId() == testUiThread);
    ++secondaryHostCalls;
    host[0] = nullptr;
    host[1] = &testControl;
    return host;
}
struct NoElement : IUnknown {
    HRESULT WINAPI QueryInterface(REFIID, void** result) override {
        *result = nullptr;
        return E_NOINTERFACE;
    }
    ULONG WINAPI AddRef() override { return 1; }
    ULONG WINAPI Release() override { return 1; }
};
void PumpWorker(HANDLE worker) {
    assert(worker);
    for (;;) {
        DWORD wait = MsgWaitForMultipleObjects(1, &worker, FALSE, 5000, QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0) break;
        assert(wait == WAIT_OBJECT_0 + 1);
        MSG message{};
        while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
    CloseHandle(worker);
}
HRESULT WINAPI TestDwm(HWND window, DWORD attribute, LPCVOID value, DWORD size) {
    assert(attribute == DWMWA_CLOAK && value && size == sizeof(BOOL));
    ++dwmCalls;
    return E_ACCESSDENIED;
}
BOOL WINAPI TestPosition(HWND window, HWND after, int x, int y, int cx, int cy, UINT flags) {
    ++positionCalls;
    forwardedFlags = flags;
    if (failPosition) return FALSE;
    BOOL result = SetWindowPos(window, after, x, y, cx, cy, flags);
    if (withdrawDuringPosition) {
        withdrawDuringPosition = false;
        RemoveProp(testBar, kTrayShift);
    }
    return result;
}
int WINAPI FakeArrange(void* self, Size size, Size* result) {
    assert(self == &owner && size.Width == 5120 && size.Height == 72);
    ++calls;
    if (!result) return E_POINTER;
    *result = size;
    return S_OK;
}
double WINAPI FakeFrameSize(int taskbarSize) {
    assert(taskbarSize >= 0 && taskbarSize <= 3);
    return 72;
}
int nativeHideCalls{}, nativeProjectedCalls{};
void* expectedNativeSelf;
const void* expectedNativeArgument;
bool throwNative;
bool expectNativeBusy;
int WINAPI FakeHideAnimation(void* self, void* element, unsigned context) {
    assert(self == expectedNativeSelf && element == expectedNativeArgument && context == 0xffffffffu);
    ++nativeHideCalls;
    return E_ACCESSDENIED;
}
void WINAPI FakeProjectedAnimation(void* self, const void* argument) {
    assert(self == expectedNativeSelf && argument == expectedNativeArgument);
    assert(g_busy == expectNativeBusy);
    ++nativeProjectedCalls;
    if (throwNative) throw E_ABORT;
}
int main() {
    wprintf(L"[UWTEST buttons] %ls\\n", EdgeExpression(true).c_str());
    wprintf(L"[UWTEST tray] %ls\\n", EdgeExpression().c_str());
    for (auto [entry, target] : {std::pair{"vs", "vs_5_0"},
                                 std::pair{"copyPs", "ps_5_0"},
                                 std::pair{"boxBlurPs", "ps_5_0"},
                                 std::pair{"ps", "ps_5_0"}}) {
        winrt::com_ptr<ID3DBlob> shader, errors;
        // Compile against the real cap, not a copy that can drift from it.
        auto capText = std::to_string(kMaxButtonLenses);
        D3D_SHADER_MACRO macros[]{
            {"BUTTON_LENS_CAP", capText.c_str()}, {nullptr, nullptr}};
        HRESULT hr = D3DCompile(LiquidRenderer::kShader,
                                sizeof(LiquidRenderer::kShader) - 1,
                                "liquid-glass", macros, nullptr, entry, target,
                                D3DCOMPILE_ENABLE_STRICTNESS, 0, shader.put(),
                                errors.put());
        if (FAILED(hr) && errors)
            fprintf(stderr, "%.*s", int(errors->GetBufferSize()),
                    static_cast<const char*>(errors->GetBufferPointer()));
        assert(SUCCEEDED(hr));
    }
    double overhang = NAN;
    assert(BoundaryThreshold(overhang, 306, 540, 1152, 840.8) == -1152);
    assert(!std::isfinite(overhang));
    double threshold = BoundaryThreshold(overhang, 252.8, 592.8, 1152, 840.8);
    assert(std::abs(threshold - 306.4) < 0.001 && std::abs(overhang - 4.8) < 0.001);
    assert(std::abs(BoundaryThreshold(overhang, 76.8, 768.8, 1152, 840.8) - threshold) < 0.001);
    assert(std::abs(BoundaryThreshold(overhang, 320, 512, 1152, 840.8) - threshold) < 0.001);
    assert(std::abs(BoundaryThreshold(overhang, 320, 512, 1152, 844) - (threshold - 3.2)) < 0.001);
    wprintf(L"[UWTEST threshold] %.6f\\n", threshold);
    Placement narrow{}, wide{};
    assert(Plan(5120, 2273.333, 823.333, 2560.333, 4296.667, -0.333, narrow));
    assert(Plan(5120, 2449.333, 823.333, 2560.333, 4296.667, -0.333, wide));
    assert(std::abs(narrow.buttonsX - wide.buttonsX) < 0.001);
    using Life = Root::Source::Life;
    assert(NextLife(Life::Active, Life::Hiding, false) == Life::Hiding);
    assert(NextLife(Life::Hiding, Life::Active, false) == Life::Hiding);
    assert(NextLife(Life::Hiding, Life::Active, true) == Life::Active);
    assert(NextLife(Life::Hiding, Life::Retired, false) == Life::Retired);
    assert(NextLife(Life::Retired, Life::Active, false) == Life::Active);
    assert(IsAttentionStateName(L"RequestingAttention"));
    assert(IsAttentionStateName(L"RequestingAttentionPointerOver"));
    assert(!IsAttentionStateName(L"ActivePointerOver"));
    assert(LiquidRetryDelay(0) == 1000);
    assert(LiquidRetryDelay(1) == 2000);
    assert(LiquidRetryDelay(5) == 32000);
    assert(LiquidRetryDelay(99) == 32000);
    RECT monitorRect{0, 0, 1920, 1080};
    assert(TaskbarAtTop({0, 0, 1920, 48}, monitorRect));
    assert(!TaskbarAtTop({0, 1032, 1920, 1080}, monitorRect));
    auto topInsets = PlanDockInsets(true, 48, 10);
    auto bottomInsets = PlanDockInsets(false, 48, 10);
    assert(topInsets.top == 10 && topInsets.bottom == 4);
    assert(bottomInsets.top == 4 && bottomInsets.bottom == 10);
    double expansion = DockFrameAdjustment(10);
    assert(expansion == 6);
    auto expandedInsets = PlanDockInsets(false, 48 + expansion, 10);
    assert(48 + expansion - expandedInsets.top - expandedInsets.bottom == 40);
    for (double edgeDistance : {0.0, 1.0, 3.0, 4.0, 10.0, 100.0}) {
        double frameHeight = 48 + DockFrameAdjustment(edgeDistance);
        for (bool taskbarAtTop : {false, true}) {
            auto insets =
                PlanDockInsets(taskbarAtTop, frameHeight, edgeDistance);
            assert(frameHeight - insets.top - insets.bottom == 40);
        }
    }
    auto plateClip = PlanVerticalClip(64, 72, 16, 68);
    assert(plateClip.X == 0 && plateClip.Y == 16 &&
           plateClip.Width == 64 && plateClip.Height == 52);
    auto clampedPlateClip = PlanVerticalClip(64, 72, -8, 80);
    assert(clampedPlateClip.Y == 0 && clampedPlateClip.Height == 72);
    assert(PlanVerticalClip(64, 72, 20, 20).Height == 0);
    auto clampedInsets = PlanDockInsets(false, 48, 100);
    assert(clampedInsets.top == 4 && clampedInsets.bottom == 43);
    TaskbarConfiguration_GetFrameSize_Original = FakeFrameSize;
    g_dockEdgeDistance = 16;
    g_dockingDisabled = false;
    double expandedFrame =
        TaskbarConfiguration_GetFrameSize_Hook(1);
    assert(expandedFrame == 84);
    auto expandedFrameInsets = PlanDockInsets(true, expandedFrame, 16);
    assert(expandedFrame - expandedFrameInsets.top -
               expandedFrameInsets.bottom ==
           64);
    assert(TaskbarConfiguration_GetFrameSize_Hook(0) == 72);
    g_dockingDisabled = true;
    assert(TaskbarConfiguration_GetFrameSize_Hook(1) == 72);
    g_dockingDisabled = false;
    g_dockEdgeDistance = 4;
    g_dockCornerRadius = 100;
    assert(EffectiveDockRadius(800, 48) == 24);
    g_dockCornerRadius = 12;
    int radii[3]{};
    FastGaussianBoxRadii(0, radii);
    assert(radii[0] == 0 && radii[1] == 0 && radii[2] == 0);
    FastGaussianBoxRadii(4, radii);
    assert(radii[0] == 3 && radii[1] == 3 && radii[2] == 4);
    FastGaussianBoxRadii(8, radii);
    assert(radii[0] == 7 && radii[1] == 7 && radii[2] == 8);
    FastGaussianBoxRadii(12, radii);
    assert(radii[0] == 11 && radii[1] == 11 && radii[2] == 12);
    assert(IncludeSource(true, Life::Active) && !IncludeSource(false, Life::Active));
    assert(IncludeSource(false, Life::Hiding) && IncludeSource(true, Life::Hiding));
    assert(!IncludeSource(true, Life::Retired) && !IncludeSource(false, Life::Retired));
    assert(WholeGenerationReplaced({1, 2, 3}, {4, 5, 6}, 52.8f, 52.8f));
    assert(!WholeGenerationReplaced({1, 2, 3}, {1, 4, 5}, 52.8f, 52.8f));
    assert(!WholeGenerationReplaced({1, 2, 3}, {4, 5}, 52.8f, 52.8f));
    assert(!WholeGenerationReplaced({1, 2, 3}, {4, 5, 6}, 52.8f, 54.0f));
    assert(EligibleSlot(3385.333, 153.333, 5122));
    assert(!EligibleSlot(-10153.333, 0, 5122));
    assert(!EligibleSlot(5000, 200, 5122));
    assert(!EligibleSlot(std::numeric_limits<double>::quiet_NaN(), 64, 5122));
    // Exercise the actual Windows expression parser with isolated Composition
    // objects. No Explorer injection, Shell mutation or visible test window.
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    auto messaging = LoadLibrary(L"CoreMessaging.dll");
    assert(messaging);
    auto createQueue = reinterpret_cast<decltype(&CreateDispatcherQueueController)>(
        GetProcAddress(messaging, "CreateDispatcherQueueController"));
    assert(createQueue);
    winrt::Windows::System::DispatcherQueueController queue{nullptr};
    winrt::check_hresult(createQueue({sizeof(DispatcherQueueOptions), DQTYPE_THREAD_CURRENT, DQTAT_COM_NONE},
        reinterpret_cast<ABI::Windows::System::IDispatcherQueueController**>(winrt::put_abi(queue))));
    ShapeVisual materialMarker{nullptr};
    {
        Compositor compositor;
        materialMarker = compositor.CreateShapeVisual();
        assert(ParseTheme(L"native") == DockTheme::Native);
        assert(ParseTheme(L"glass") == DockTheme::Glass);
        assert(ParseTheme(L"frosted") == DockTheme::Frosted);
        assert(ParseTheme(L"prism") == DockTheme::Native);
        assert(ParseTheme(L"unexpected") == DockTheme::Native);
        assert(IsTaskbarIconSizeModuleName(
            L"taskbar-icon-size_1.3.10_648251.dll"));
        assert(IsTaskbarIconSizeModuleName(
            L"local@taskbar-icon-size_1.3.10_648251.dll"));
        assert(!IsTaskbarIconSizeModuleName(
            L"windows-11-better-ultrawide-taskbar_2.33.dll"));
        LiquidSettings custom;
        custom.blur = 0.77f;
        custom.brightness = 0.25f;
        assert(ApplyLiquidPreset(L"custom", custom).blur == 0.77f);
        // Regular is ybouane/liquidglass DEFAULTS untouched; only Frosted and
        // Dark carry the reference site's 0.25 backdrop blur.
        auto regular = ApplyLiquidPreset(L"regular", custom);
        assert(regular.blur == 0 && regular.brightness == 0);
        assert(regular.refraction == 0.69f && regular.chroma == 0.05f &&
               regular.edgeHighlight == 0.05f && regular.specular == 0 &&
               regular.fresnel == 1 && regular.opacity == 1 &&
               regular.zRadius == 40);
        auto frosted = ApplyLiquidPreset(L"frosted", custom);
        assert(frosted.blur == 0.25f && frosted.brightness == 0);
        auto dark = ApplyLiquidPreset(L"dark", custom);
        assert(dark.blur == 0.25f && dark.brightness == -0.3f);
        {
            auto outline = compositor.CreateRoundedRectangleGeometry();
            auto highlights = GlassHighlights(outline, DockTheme::Liquid, false);
            assert(highlights.Shapes().Size() == 0);
        }
        for (auto theme : {DockTheme::Glass, DockTheme::Frosted}) {
            for (bool dark : {false, true}) {
                auto outline = compositor.CreateRoundedRectangleGeometry();
                outline.Size({900, 64});
                outline.Offset({200, 4});
                auto highlights = GlassHighlights(outline, theme, dark);
                assert(highlights.Shapes().Size() == 1);
                assert(highlights.RelativeSizeAdjustment().x == 1 &&
                       highlights.RelativeSizeAdjustment().y == 1);
                auto shape = highlights.Shapes().GetAt(0).as<CompositionSpriteShape>();
                assert(shape.Geometry() == outline && shape.StrokeThickness() == 2);
                auto rim = shape.StrokeBrush().as<CompositionLinearGradientBrush>();
                assert(rim.ColorStops().Size() == 4);
                assert(rim.ColorStops().GetAt(1).Color().B ==
                       rim.ColorStops().GetAt(1).Color().R);
                // The production decoration uses the animated clip object itself:
                // both continue to share it after resize and motion.
                outline.Size({1200, 64});
                outline.Offset({50, 4});
                assert(shape.Geometry().as<CompositionRoundedRectangleGeometry>().Size().x == 1200);
                assert(shape.Geometry().as<CompositionRoundedRectangleGeometry>().Offset().x == 50);
            }
        }
        // Exercise the production teardown on real Composition objects: a
        // removed dock must not leave its crop mask attached to the background.
        auto surface = compositor.CreateSpriteVisual();
        for (bool savedClip : {false, true}) {
            CompositionClip original = savedClip ? compositor.CreateInsetClip() : nullptr;
            for (int reload = 0; reload < 3; ++reload) {
                OwnedDock owned;
                owned.visual = surface;
                owned.originalClip = original;
                owned.geometry = compositor.CreateRoundedRectangleGeometry();
                owned.clip = compositor.CreateGeometricClip(owned.geometry);
                surface.Clip(owned.clip);
                auto animation = compositor.CreateExpressionAnimation(L"Vector2(100, 4)");
                owned.geometry.StartAnimation(L"Offset", animation);
                Restore(owned);
                assert(surface.Clip() == original && !owned.clip);
                Restore(owned);
                assert(surface.Clip() == original);
            }
        }
        {
            OwnedDock owned;
            owned.visual = surface;
            owned.geometry = compositor.CreateRoundedRectangleGeometry();
            owned.clip = compositor.CreateGeometricClip(owned.geometry);
            auto external = compositor.CreateInsetClip();
            surface.Clip(external);
            Restore(owned);
            assert(surface.Clip() == external);
        }
        wprintf(L"[UWTEST dock] clip restoration and repeated unload passed\\n");
        auto metrics = compositor.CreatePropertySet();
        metrics.InsertScalar(L"floor", 100);
        metrics.InsertScalar(L"threshold", 306.4f);
        metrics.InsertScalar(L"shift", -200);
        metrics.InsertScalar(L"prefix", 65);
        metrics.InsertScalar(L"trayBase", -400);
        metrics.InsertScalar(L"trayDockBase", -1200);
        metrics.InsertScalar(L"minX", -4000);
        metrics.InsertScalar(L"maxX", 0);
        metrics.InsertScalar(L"dockOrigin", 0);
        metrics.InsertScalar(L"center", 2560);
        metrics.InsertScalar(L"padding", 8);
        metrics.InsertScalar(L"margin", 4);
        metrics.InsertScalar(L"height", 64);
        metrics.InsertScalar(L"vertical", 6);
        metrics.InsertScalar(L"gap", 12);
        metrics.InsertScalar(L"trayWidth", 400);
        auto anchor = compositor.CreateSpriteVisual();
        anchor.Offset({200,0,0});
        auto handoff = compositor.CreatePropertySet();
        handoff.InsertScalar(L"value", 0);
        auto dockReference = compositor.CreateRoundedRectangleGeometry();
        dockReference.Offset({100, 0});
        dockReference.Size({1000, 64});
        auto output = compositor.CreateSpriteVisual();
            for (bool buttons : {false, true}) {
            try {
            auto expression =
                compositor.CreateExpressionAnimation(EdgeExpression(buttons));
            expression.SetReferenceParameter(L"geometry", metrics);
            expression.SetReferenceParameter(L"anchor", anchor);
            if (!buttons) {
                expression.SetReferenceParameter(L"handoff", handoff);
                expression.SetReferenceParameter(L"dock", dockReference);
            }
            output.StartAnimation(L"Offset", expression);
            auto dock = compositor.CreateRoundedRectangleGeometry();
            std::wstring dockLeft =
                L"geometry.dockOrigin + geometry.shift + Max(0, (geometry.threshold - "
                L"(anchor.Offset.X - geometry.prefix)) / 2) + anchor.Offset.X - "
                L"geometry.prefix - geometry.padding";
            auto dockOffset = compositor.CreateExpressionAnimation(
                L"Vector2(" + dockLeft + L", geometry.margin)");
            auto dockSize = compositor.CreateExpressionAnimation(
                L"Vector2(2 * Max(0, geometry.center - (" + dockLeft +
                L")), geometry.height)");
            for (auto const& dockExpression : {dockOffset, dockSize}) {
                dockExpression.SetReferenceParameter(L"geometry", metrics);
                dockExpression.SetReferenceParameter(L"anchor", anchor);
            }
            dock.StartAnimation(L"Offset", dockOffset);
            dock.StartAnimation(L"Size", dockSize);
            output.StopAnimation(L"Offset");
            } catch (winrt::hresult_error const& error) {
                fwprintf(stderr, L"Expression hr=%08X: %ls",
                         static_cast<unsigned>(error.code().value), error.message().c_str());
                return 1;
            }
            }
    }
    auto shutdown = queue.ShutdownQueueAsync();
    while (shutdown.Status() == winrt::Windows::Foundation::AsyncStatus::Started) {
        MSG message{};
        while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) DispatchMessage(&message);
        Sleep(1);
    }
    shutdown.GetResults();
    // Exercise raw ABI vs projected-pointer forwarding, rejected element QI,
    // stopped/trace-limit pass-through, original failure and exception propagation.
    HideAnimation_Original = FakeHideAnimation;
    StartAnimation_Original = FakeProjectedAnimation;
    HideCompleted_Original = FakeProjectedAnimation;
    NoElement unsupported;
    void* projected = &unsupported;
    g_ownerThread = GetCurrentThreadId();
    expectedNativeSelf = &owner;
    expectedNativeArgument = &unsupported;
    assert(HideAnimation_Hook(expectedNativeSelf, &unsupported, 0xffffffffu) == E_ACCESSDENIED);
    g_stopping = true;
    assert(HideAnimation_Hook(expectedNativeSelf, &unsupported, 0xffffffffu) == E_ACCESSDENIED);
    g_stopping = false;
    expectedNativeSelf = &projected;
    expectedNativeArgument = &owner;
    StartAnimation_Hook(expectedNativeSelf, expectedNativeArgument);
    StartAnimation_Hook(expectedNativeSelf, expectedNativeArgument);
    expectedNativeSelf = &owner;
    expectedNativeArgument = &projected;
    expectNativeBusy = true;
    HideCompleted_Hook(expectedNativeSelf, expectedNativeArgument);
    HideCompleted_Hook(expectedNativeSelf, expectedNativeArgument);
    throwNative = true;
    bool propagated = false;
    try { HideCompleted_Hook(expectedNativeSelf, expectedNativeArgument); }
    catch (HRESULT error) { propagated = error == E_ABORT; }
    assert(propagated && nativeHideCalls == 2 && nativeProjectedCalls == 5 &&
           g_active == 0 && !g_busy);
    throwNative = false;
    expectNativeBusy = false;
    g_ownerThread = 0;
    // Fail closed on changed private code, unreadable data, and missing XAML;
    // release the native shared_ptr even when root discovery cannot succeed.
    BYTE prologue[8]{0x48,0x83,0xEC,0x28,0x48,0x83,0xC1,0x48};
    size_t offset = 123;
    assert(HostElementOffset(prologue, offset) && offset == 0x48);
    for (BYTE invalid : {0, 7, 9, 0x80, 0xF8}) {
        prologue[7] = invalid;
        assert(!HostElementOffset(prologue, offset) && offset == 0x48);
    }
    prologue[7] = 0x48;
    prologue[6] = 0xC2;
    assert(!HostElementOffset(prologue, offset) && offset == 0x48);
    void* slots[21]{};
    g_taskBandVtable = &testControl;
    slots[19] = g_taskBandVtable;
    assert(TaskBandInterface(slots, g_taskBandVtable) == &slots[19]);
    slots[19] = nullptr;
    slots[20] = g_taskBandVtable;
    assert(!TaskBandInterface(slots, g_taskBandVtable) && !TaskBandInterface(nullptr, g_taskBandVtable));
    void* unreadable = VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_NOACCESS);
    assert(unreadable && !TaskBandInterface(unreadable, g_taskBandVtable));
    Decref_Original = TestDecref;
    g_hostElementOffset = sizeof(void*);
    void* host[2]{nullptr, &testControl};
    assert(!RootFromHost(host) && hostReleases == 1);
    host[0] = unreadable;
    assert(!RootFromHost(host) && hostReleases == 2);
    assert(VirtualFree(unreadable, 0, MEM_RELEASE));
    NoElement noElement;
    void* hostObject[2]{nullptr, &noElement};
    host[0] = hostObject;
    assert(!RootFromHost(host) && hostReleases == 3);
    host[1] = nullptr;
    assert(!RootFromHost(host) && hostReleases == 3);
    // Captured hosts, smaller unrelated ActionCenter, negative monitor origins,
    // repeated positions, changed widths, clamping and signed property encoding.
    RECT work{0, 108, 7680, 2160};
    int nativeX{}, panelX{};
    for (int shift : {-1707, 0, 500, -8000}) {
        assert(DecodeShift(EncodeShift(shift)) == shift);
    }
    assert(PlanPanel(work, {7104,108,7680,2160}, -1707, nullptr, nativeX, panelX));
    assert(nativeX == 7104 && panelX == 5397);
    HANDLE prior = EncodePosition(nativeX, panelX);
    for (int i = 0; i < 20; ++i) {
        assert(PlanPanel(work, {panelX,108,panelX+576,2160}, -1707, prior, nativeX, panelX));
        assert(nativeX == 7104 && panelX == 5397);
    }
    assert(PlanPanel(work, {7104,108,7680,2160}, -1707, prior, nativeX, panelX));
    assert(panelX == 5397);
    assert(PlanPanel(work, {5397,108,5997,2160}, -1707, prior, nativeX, panelX));
    assert(nativeX == 7080 && panelX == 5373); // Native width change, same position.
    assert(PlanPanel(work, {7140,108,7680,2160}, -1707, nullptr, nativeX, panelX));
    assert(panelX == 5433);
    assert(!PlanPanel(work, {3543,108,4137,672}, -1707, nullptr, nativeX, panelX));
    assert(!PlanPanel(work, {6000,108,6576,2160}, -1707, prior, nativeX, panelX));
    assert(!PlanPanel(work, {0,108,7680,2160}, -1707, nullptr, nativeX, panelX));
    assert(PlanPanel(work, {7104,108,7680,2160}, -7600, nullptr, nativeX, panelX) && panelX == 0);
    assert(PlanPanel(work, {7104,108,7680,2160}, 100, nullptr, nativeX, panelX) && panelX == 7104);
    assert(!PlanPanel(work, {7104,108,7680,2160}, -8000, nullptr, nativeX, panelX));
    assert(PlanPanel({-2560,-1440,0,-40}, {-400,-1440,0,-40}, -300, nullptr, nativeX, panelX));
    assert(nativeX == -400 && panelX == -700);
    int restoredX{}, appliedX{};
    DecodePosition(EncodePosition(nativeX, panelX), restoredX, appliedX);
    assert(restoredX == -400 && appliedX == -700);
    SetWindowPos_Original = SetWindowPos;
    DestroyWindow_Original = DestroyWindow;
    DwmSetWindowAttribute_Original = TestDwm;
    BOOL uncloaked = FALSE;
    assert(PanelDwmAttribute(nullptr, DWMWA_CLOAK, &uncloaked, sizeof(uncloaked)) == E_ACCESSDENIED);
    assert(dwmCalls == 1 && g_active == 0);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    WNDCLASS panelClass{};
    panelClass.lpfnWndProc = DefWindowProc;
    panelClass.hInstance = GetModuleHandle(nullptr);
    panelClass.lpszClassName = L"ControlCenterWindow";
    assert(RegisterClass(&panelClass));
    WNDCLASS barClass = panelClass;
    barClass.lpszClassName = L"Shell_TrayWnd";
    barClass.cbWndExtra = sizeof(void*);
    assert(RegisterClass(&barClass));
    MONITORINFO monitor{};
    monitor.cbSize = sizeof(monitor);
    assert(GetMonitorInfo(MonitorFromPoint({0,0}, MONITOR_DEFAULTTOPRIMARY), &monitor));
    RECT area = monitor.rcWork;
    int edge = area.right - 400;
    auto makeWindow = [&](const wchar_t* name, int x, int width) {
        return CreateWindowEx(WS_EX_TOOLWINDOW, name, L"", WS_POPUP, x, area.top,
                              width, area.bottom-area.top, nullptr, nullptr, panelClass.hInstance, nullptr);
    };
    testBar = makeWindow(barClass.lpszClassName, area.left, 40);
    HWND panel = makeWindow(panelClass.lpszClassName, edge, 400);
    assert(testBar && panel && IsPanel(panel));
    RECT monitorRectPixels = monitor.rcMonitor;
    RECT nativeBar{monitorRectPixels.left, monitorRectPixels.bottom - 48,
                   monitorRectPixels.right, monitorRectPixels.bottom};
    assert(SetWindowPos(testBar, nullptr, nativeBar.left, nativeBar.top,
                        nativeBar.right - nativeBar.left,
                        nativeBar.bottom - nativeBar.top,
                        SWP_NOZORDER | SWP_NOACTIVATE));
    g_dockEdgeDistance = 4;
    // Exercise discovery on this process's hidden test windows only. The
    // temporary hook must dispatch to the owner thread and then be removed.
    g_discoverMessage = RegisterWindowMessage(L"Windhawk.UWPOS06.CheckDiscovery");
    GetTaskbarHost_Original = TestGetHost;
    testUiThread = GetCurrentThreadId();
    slots[0] = g_taskBandVtable;
    testSite = slots;
    SetWindowLongPtr(testBar, 0, reinterpret_cast<LONG_PTR>(slots));
    assert(SetProp(testBar, L"TaskbandHWND", testBar));
    assert(!RequestDiscovery(panel) && !RequestDiscovery(nullptr));
    assert(RequestDiscovery(testBar) && hostCalls == 1 && hostReleases == 4);
    PumpWorker(CreateThread(nullptr, 0, [](void*) -> DWORD {
        assert(!ExistingRoot(testBar)); // Wrong thread cannot read XAML.
        assert(RequestDiscovery(testBar));
        SendMessage(testBar, g_discoverMessage,
                    reinterpret_cast<WPARAM>(&g_discoverMessage), 0);
        return 0;
    }, nullptr, 0, nullptr));
    assert(hostCalls == 2 && hostReleases == 5 && g_active == 0);
    Wh_ModAfterInit(); // Shell-host mode must not run Explorer discovery.
    assert(hostCalls == 2);
    g_explorer = true;
    g_taskbarViewReady = true;
    Wh_ModAfterInit(); // Enumeration filters out other processes' real taskbars.
    assert(hostCalls == 3 && hostReleases == 6);
    // Secondary lookup must use its WorkerW child, distinct vtable and getter.
    WNDCLASS secondaryClass = barClass;
    secondaryClass.lpszClassName = L"Shell_SecondaryTrayWnd";
    assert(RegisterClass(&secondaryClass));
    WNDCLASS workerClass = barClass;
    workerClass.lpszClassName = L"WorkerW";
    assert(RegisterClass(&workerClass));
    testSecondary = makeWindow(secondaryClass.lpszClassName, area.left, 40);
    assert(testSecondary);
    g_secondaryTaskBandVtable = &secondaryHostCalls;
    GetSecondaryTaskbarHost_Original = TestGetSecondaryHost;
    assert(RequestDiscovery(testSecondary) && secondaryHostCalls == 0);
    HWND band = CreateWindowEx(0, workerClass.lpszClassName, L"", WS_CHILD,
                               0, 0, 1, 1, testSecondary, nullptr, workerClass.hInstance, nullptr);
    assert(band);
    SetWindowLongPtr(band, 0, reinterpret_cast<LONG_PTR>(slots));
    assert(RequestDiscovery(testSecondary) && secondaryHostCalls == 0); // Wrong vtable.
    void* secondarySlots[20]{g_secondaryTaskBandVtable};
    testSecondarySite = secondarySlots;
    SetWindowLongPtr(band, 0, reinterpret_cast<LONG_PTR>(secondarySlots));
    assert(RequestDiscovery(testSecondary) && secondaryHostCalls == 1 && hostReleases == 7);
    PumpWorker(CreateThread(nullptr, 0, [](void*) -> DWORD {
        assert(!ExistingRoot(testSecondary));
        assert(RequestDiscovery(testSecondary));
        SendMessage(testSecondary, g_discoverMessage,
                    reinterpret_cast<WPARAM>(&g_discoverMessage), 0);
        return 0;
    }, nullptr, 0, nullptr));
    assert(secondaryHostCalls == 2 && hostReleases == 8 && g_active == 0);
    Wh_ModAfterInit();
    assert(hostCalls == 4 && secondaryHostCalls == 3 && hostReleases == 10);
    assert(DestroyWindow(testSecondary));
    assert(UnregisterClass(workerClass.lpszClassName, workerClass.hInstance));
    assert(UnregisterClass(secondaryClass.lpszClassName, secondaryClass.hInstance));
    g_explorer = false;
    RemoveProp(testBar, L"TaskbandHWND");
    assert(SetProp(testBar, kTrayShift, EncodeShift(-300)));
    SetWindowPos_Original = TestPosition;
    UINT nativeFlags = SWP_NOZORDER | SWP_NOACTIVATE;
    assert(PanelSetWindowPos(panel, nullptr, edge, area.top, 400, area.bottom-area.top, nativeFlags));
    RECT actual{};
    assert(GetWindowRect(panel, &actual) && actual.left == edge-300 && actual.top == area.top &&
           actual.right-actual.left == 400 && actual.bottom == area.bottom);
    assert(forwardedFlags == nativeFlags && positionCalls == 1);
    assert(GetProp(panel, kPanelTaskbar) == testBar && GetProp(panel, kPanelPosition));
    assert(PanelSetWindowPos(panel, nullptr, 0, 0, 0, 0, nativeFlags | SWP_NOMOVE | SWP_NOSIZE));
    assert(GetWindowRect(panel, &actual) && actual.left == edge-300 && positionCalls == 2);
    failPosition = true;
    assert(!PanelSetWindowPos(panel, nullptr, edge, area.top, 400, area.bottom-area.top, nativeFlags));
    failPosition = false;
    assert(GetProp(panel, kPanelPosition));
    RestorePanel(panel);
    assert(GetWindowRect(panel, &actual) && actual.left == edge && !GetProp(panel, kPanelPosition));
    // Replay the captured first-open sequence: pre-uncloak sees 1x1, then a
    // location event exposes the final native rectangle without SetWindowPos.
    assert(SetWindowPos(panel, nullptr, edge, area.top, 1, 1, nativeFlags));
    assert(PanelDwmAttribute(panel, DWMWA_CLOAK, &uncloaked, sizeof(uncloaked)) == E_ACCESSDENIED);
    assert(GetProp(panel, kPanelPending));
    assert(SetWindowPos(panel, nullptr, edge, area.top, 400, area.bottom-area.top, nativeFlags));
    PanelLocationEvent(nullptr, EVENT_OBJECT_LOCATIONCHANGE, panel, OBJID_CLIENT,
                       CHILDID_SELF, 0, 0);
    assert(GetWindowRect(panel, &actual) && actual.left == edge && GetProp(panel, kPanelPending));
    PanelLocationEvent(nullptr, EVENT_OBJECT_LOCATIONCHANGE, panel, OBJID_WINDOW,
                       CHILDID_SELF, 0, 0);
    assert(GetWindowRect(panel, &actual) && actual.left == edge-300 && !GetProp(panel, kPanelPending));
    RestorePanel(panel);
    assert(GetWindowRect(panel, &actual) && actual.left == edge);
    // Query replies can differ from the layout publication during animation.
    // Their position must survive the post-call publication-availability check.
    WNDCLASS queryClass{};
    queryClass.hInstance = GetModuleHandle(nullptr);
    queryClass.lpszClassName = kWindowClass;
    queryClass.lpfnWndProc = [](HWND window, UINT message, WPARAM wp, LPARAM lp) -> LRESULT {
        if (message == kQueryTrayMessage) {
            assert(reinterpret_cast<HWND>(wp) == testBar);
            return reinterpret_cast<LRESULT>(EncodeShift(-450));
        }
        return DefWindowProc(window, message, wp, lp);
    };
    assert(RegisterClass(&queryClass));
    HWND query = CreateWindowEx(0, kWindowClass, L"", 0, 0, 0, 0, 0,
                                HWND_MESSAGE, nullptr, queryClass.hInstance, nullptr);
    assert(query && SetProp(testBar, kTrayDispatcher, query));
    assert(QueryTrayShift(testBar) == EncodeShift(-450));
    assert(PanelSetWindowPos(panel, nullptr, edge, area.top, 400, area.bottom-area.top, nativeFlags));
    assert(GetWindowRect(panel, &actual) && actual.left == edge-450 && GetProp(panel, kPanelPosition));
    RestorePanel(panel);
    assert(GetWindowRect(panel, &actual) && actual.left == edge);
    assert(SetProp(testBar, kTrayDispatcher, panel)); // Wrong dispatcher class.
    assert(!QueryTrayShift(testBar));
    RemoveProp(testBar, kTrayDispatcher);
    assert(QueryTrayShift(testBar) == EncodeShift(-300));
    assert(DestroyWindow(query) && UnregisterClass(kWindowClass, queryClass.hInstance));
    assert(PanelSetWindowPos(panel, nullptr, edge, area.top, 400, area.bottom-area.top, nativeFlags));
    assert(SetWindowPos(panel, nullptr, edge-200, area.top, 0, 0, nativeFlags | SWP_NOSIZE));
    RestorePanel(panel); // A later external X write must survive restoration.
    assert(GetWindowRect(panel, &actual) && actual.left == edge-200 && !GetProp(panel, kPanelPosition));
    withdrawDuringPosition = true;
    assert(PanelSetWindowPos(panel, nullptr, edge, area.top, 400, area.bottom-area.top, nativeFlags));
    assert(GetWindowRect(panel, &actual) && actual.left == edge && !GetProp(panel, kPanelPosition));
    assert(SetProp(testBar, kTrayShift, EncodeShift(-300)));
    assert(PanelSetWindowPos(panel, nullptr, edge, area.top, 400, area.bottom-area.top, nativeFlags));
    assert(SetProp(testBar, kTrayShift, EncodeShift(0)));
    assert(PanelSetWindowPos(panel, nullptr, 0, 0, 0, 0, nativeFlags | SWP_NOMOVE | SWP_NOSIZE));
    assert(GetWindowRect(panel, &actual) && actual.left == edge && !GetProp(panel, kPanelPosition));
    assert(SetProp(testBar, kTrayShift, EncodeShift(-300)));
    assert(PanelSetWindowPos(panel, nullptr, edge, area.top, 400, area.bottom-area.top, nativeFlags));
    // Publisher teardown restores the tagged panel before destroying its HWND.
    assert(PanelDestroyWindow(testBar));
    assert(GetWindowRect(panel, &actual) && actual.left == edge && !GetProp(panel, kPanelPosition));
    assert(PanelDestroyWindow(panel));
    assert(UnregisterClass(panelClass.lpszClassName, panelClass.hInstance));
    assert(UnregisterClass(barClass.lpszClassName, barClass.hInstance));
    SetWindowPos_Original = SetWindowPos;
    Placement opened{}, closed{}, unchanged{123, 456};
    // Secondary regression: final-layout compensation jumps 88 DIPs
    // while the native Start visual is still at its previous position. This
    // characterizes the unsupported static-layout behavior.
    Placement secondaryBefore{}, secondaryAfter{};
    assert(Plan(1152, 592.8, 311.2, 576, 840.8, 252.8-279.6, secondaryBefore));
    assert(Plan(1152, 768.8, 311.2, 576, 840.8, 76.8-191.6, secondaryAfter));
    assert(std::abs(secondaryAfter.buttonsX-secondaryBefore.buttonsX-88) < 0.001);
    assert(76.8 + secondaryBefore.buttonsX < 0); // Exit applies the smaller strip's shift too soon.
    assert(Plan(5120, 1912.667, 791.333, 2560.333, 4328.667, 0, opened));
    assert(Plan(5120, 1807.333, 791.333, 2560.333, 4328.667, 0, closed));
    assert(std::abs(opened.buttonsX - closed.buttonsX) < 0.001);
    double buttonLeft = 2560.333 - 1912.667 / 2 + opened.buttonsX;
    double buttonRight = buttonLeft + 1912.667;
    double trayLeft = 4328.667 + opened.trayX;
    assert(std::abs(trayLeft - buttonRight - 12) < 0.001);
    assert(std::abs((buttonLeft + trayLeft + 791.333) / 2 - 2560) < 0.001);
    assert(!Plan(1600, 1912.667, 791.333, 800, 808.667, 0, unchanged));
    assert(!Plan(5120, 0, 791.333, 2560, 4328.667, 0, unchanged));
    assert(!Plan(5120, 1000, 300, 2000, 4820, 0, unchanged));
    assert(!Plan(5120, std::numeric_limits<double>::quiet_NaN(), 300, 2560, 4820, 0, unchanged));
    assert(!Plan(1152, 540, 314.4, 576, 837.6, 2, unchanged));
    assert(!Plan(1152, 540, 314.4, 576, 837.6, -600, unchanged));
    assert(!Plan(1152, 540, 314.4, 576, 837.6, std::numeric_limits<double>::quiet_NaN(), unchanged));
    assert(unchanged.buttonsX == 123 && unchanged.trayX == 456);
    // Capture 20's three rejected native layouts plus both recovery states.
    // Check the resulting span, not just the computed translation constants.
    struct Sample { double buttons, tray, start; };
    for (auto sample : {Sample{652,307.2,196.8}, Sample{685.6,307.2,163.2},
                        Sample{540,314.4,302.4}, Sample{540,311.2,305.6},
                        Sample{540,307.2,306.4}}) {
        Placement placed{};
        double delta = sample.start - (1152-sample.buttons)/2;
        assert(Plan(1152, sample.buttons, sample.tray, 576, 1152-sample.tray, delta, placed));
        double left = sample.start + placed.buttonsX;
        double trayStart = 1152-sample.tray + placed.trayX;
        assert(left >= 0 && trayStart + sample.tray <= 1152);
        assert(std::abs(trayStart - left - sample.buttons - 12) < 0.001);
        assert(std::abs(left - (1152-trayStart-sample.tray)) < 0.001);
    }
    Arrange_Original = FakeArrange;
    g_stopping = true;
    Size result{};
    assert(Arrange_Hook(&owner, {5120,72}, &result) == S_OK);
    assert(result.Width == 5120 && result.Height == 72);
    g_stopping = false;
    assert(Arrange_Hook(&owner, {5120,72}, nullptr) == E_POINTER);
    assert(calls == 2 && g_active == 0);
    WNDCLASS wc{};
    wc.lpfnWndProc = DispatchProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = kWindowClass;
    g_windowClass = RegisterClass(&wc);
    assert(g_windowClass && EnsureDispatcher());
    HWND window = g_dispatch;
    assert(IsWindow(window));
    Root deferredLiquid;
    deferredLiquid.taskbar = testBar;
    deferredLiquid.publishedShift = reinterpret_cast<HANDLE>(1);
    deferredLiquid.dock.highlights = materialMarker;
    g_theme = DockTheme::Liquid;
    // Values are inert: this exercises the deferred-initialisation return.
    PrepareLiquid(deferredLiquid, Measurements{.host = 5120,
                                              .trayWidth = 600,
                                              .buttonsWidth = 800});
    assert(g_liquidPreparePending && !deferredLiquid.dock.liquid);
    MSG posted{};
    assert(PeekMessage(&posted, window, kPrepareLiquidMessage,
                       kPrepareLiquidMessage, PM_REMOVE));
    assert(posted.message == kPrepareLiquidMessage);
    g_liquidPreparePending = false;
    HANDLE worker = CreateThread(nullptr, 0, [](void*) -> DWORD {
        assert(!EnsureDispatcher()); // A second thread cannot adopt UI state.
        Wh_ModBeforeUninit();
        return 0;
    }, nullptr, 0, nullptr);
    PumpWorker(worker);
    assert(g_stopping && !g_dispatch && !IsWindow(window));
    Wh_ModUninit();
    WNDCLASS found{};
    assert(!GetClassInfo(wc.hInstance, kWindowClass, &found));
}
''', encoding="utf-8")
executable = folder / "candidate.check.exe"
subprocess.run([*driver, "-DWH_EDITING", "-static", "-O2", str(harness), *libs,
                "-o", str(executable)], check=True)
result = subprocess.run(["rtk", "proxy", str(executable)], check=True,
                        stdout=subprocess.PIPE, text=True, encoding="utf-8")
# Evaluate the actual expression strings emitted by the C++ implementation
# against captured in-flight inputs. Windows above validates the real grammar;
# this check validates centering/gap rather than merely accepting the parser.
from types import SimpleNamespace as NS
expressions = {name: next(line.split("] ", 1)[1] for line in result.stdout.splitlines()
                         if line.startswith(f"[UWTEST {name}] "))
               for name in ("buttons", "tray")}
threshold = float(next(line.split("] ", 1)[1] for line in result.stdout.splitlines()
                       if line.startswith("[UWTEST threshold] ")))


def animated_dock(geometry, left):
    correction = max(0, (geometry.threshold - left) / 2)
    offset = (geometry.dockOrigin + geometry.shift + correction + left -
              geometry.padding)
    return NS(Offset=NS(X=offset),
              Size=NS(X=2 * max(0, geometry.center - offset)))


# Cross the inferred native boundary in both directions with one expression.
# Inputs below are a model of native reflow, not recorded compositor frames.
for lefts in ([320, 310, threshold, 300, 252.8, 164.8, 76.8],
              [76.8, 164.8, 252.8, 300, threshold, 310, 320]):
    positions = []
    geometry = NS(shift=-161.6, trayBase=-161.6+12-840.8, threshold=threshold,
                  prefix=64.8, floor=64.8, minX=-840.8, maxX=0,
                  trayDockBase=-1160, dockOrigin=0, center=576, padding=8,
                  vertical=0)
    for left in lefts:
        right = min(1152-left, 845.6)
        context = dict(geometry=geometry, anchor=NS(Offset=NS(X=left+geometry.prefix)),
                       dock=animated_dock(geometry, left),
                       handoff=NS(value=0), Max=max,
                       Clamp=lambda value, low, high: min(max(value, low), high),
                       Vector3=lambda x, y, z: x)
        shifts = {name: eval(expr, {"__builtins__": {}}, context)
                  for name, expr in expressions.items()}
        visible_left = left + shifts["buttons"]
        visible_right = right + shifts["buttons"]
        tray_left = 840.8 + shifts["tray"]
        positions.append(visible_left)
        assert visible_left >= 0 and tray_left + 311.2 <= 1152
        assert abs(tray_left - visible_right - 12) < 0.001
        assert abs(visible_left - (1152-tray_left-311.2)) < 0.001
    assert positions == sorted(positions, reverse=lefts[0] > lefts[-1])
    # At first calibration the old centered anchor still has zero correction.
    context['anchor'].Offset.X = 306.4 + geometry.prefix
    assert abs(eval(expressions['buttons'], {'__builtins__': {}}, context) - geometry.shift) < 0.001
print("PASS: boundary calibration retained across exit/tray resize; continuous model crosses both ways without clipping, 12-DIP gap and symmetric settled margins.")
# The centered parent and tray must follow the same dock even while native
# right edges independently reflow or retire. The parent expression carries no
# right-edge term at all now, so that independence is structural, not sampled.
for left, right in ((1424, 3697.333), (1336, 3785.333), (1380, 3785.333),
                    (1424, 3785.333), (1424, 3697.333)):
    origin, shift, tray_left = -0.667, -417.6665, 4296.667
    tray_width = 823.333
    geometry = NS(shift=shift, trayBase=origin+shift+12-tray_left, threshold=800,
                  prefix=65.333, floor=65.333, minX=-tray_left, maxX=0,
                  trayDockBase=-5128, dockOrigin=origin, center=2560,
                  padding=8, vertical=0)
    context = dict(geometry=geometry, anchor=NS(Offset=NS(X=left+geometry.prefix)),
                   dock=animated_dock(geometry, left),
                   handoff=NS(value=0), Max=max,
                   Clamp=lambda value, low, high: min(max(value, low), high),
                   Vector3=lambda x, y, z: x)
    parent = eval(expressions["buttons"], {"__builtins__": {}}, context)
    tray_shift = eval(expressions["tray"], {"__builtins__": {}}, context)
    assert parent == shift
    assert abs(tray_left + tray_shift -
               (context["dock"].Offset.X + context["dock"].Size.X -
                geometry.padding - tray_width)) < 0.001
print("PASS: centered parent stays independent of reflow/retirement; tray follows the animated dock.")
print("PASS: real-API x64 build; guarded startup lookup, deferred optical initialization, shared_ptr release, owner-thread dispatch and hook removal; taskbar/panel geometry, signed offsets, repeat/resize/clamp/fallback; isolated HWND placement/restore/external-write/failure/withdrawal; forwarding and dispatcher teardown.")
print("Not tested: live XAML ownership/restoration, hit targets, native/companion labels, or animations.")
print("PASS: native animation hooks preserve ABI/projected pointers, original failure/exception, and exactly-once forwarding during inspection rejection, cutoff and stopping.")
print("PASS: retirement/reuse/cancel state and parked-slot rejection; Windows Composition accepts the dock and parent expressions in the isolated process.")
print("PASS: dock clip detach restores null/non-null clips over repeated unloads and preserves external clips.")
print("PASS: theme parsing and glass highlights in light/dark mode use the live dock geometry.")
print("PASS: popup query uses the sampled offset, rejects a wrong dispatcher, and preserves restoration when the layout publication differs.")
