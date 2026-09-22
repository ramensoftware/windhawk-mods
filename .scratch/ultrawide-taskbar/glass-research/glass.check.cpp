#include "../../../mods/windows-11-better-ultrawide-taskbar.wh.cpp"
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <cassert>
#include <cstdio>
int main() {
    try {
        winrt::init_apartment(winrt::apartment_type::single_threaded);
        auto manager = winrt::Windows::UI::Xaml::Hosting::WindowsXamlManager::InitializeForCurrentThread();
        {
            Controls::Border background;
            Controls::Grid grid;
            Shapes::Rectangle fill, stroke;
            fill.Name(L"BackgroundFill");
            stroke.Name(L"BackgroundStroke");
            grid.Children().Append(fill);
            grid.Children().Append(stroke);
            background.Child(grid);
            OwnedDock dock;
            dock.background = background;
            auto visual = Hosting::ElementCompositionPreview::GetElementVisual(background);
            dock.geometry = visual.Compositor().CreateRoundedRectangleGeometry();
            dock.geometry.Size({900, 64});
            Media::SolidColorBrush original(winrt::Windows::UI::Color{255, 40, 50, 60});
            for (int localBrush : {0, 1, 2}) {
                if (localBrush == 1) fill.Fill(original);
                else if (localBrush == 2) fill.Fill(nullptr);
                else fill.ClearValue(Shapes::Shape::FillProperty());
                auto before = fill.ReadLocalValue(Shapes::Shape::FillProperty());
                for (auto theme : {DockTheme::Glass, DockTheme::Frosted}) {
                    for (int cycle = 0; cycle < 3; ++cycle) {
                        g_theme = theme;
                        PrepareGlass(dock);
                        assert(dock.material && !dock.materialBlocked);
                        assert(fill.Fill() == dock.material && stroke.Fill() == dock.transparentStroke);
                        assert(Hosting::ElementCompositionPreview::GetElementChildVisual(background) == dock.highlights);
                        assert(std::isnan(background.Width()) && background.Margin() == Thickness{});
                        auto material = dock.material;
                        PrepareGlass(dock);
                        assert(dock.material == material); // No per-layout recreation.
                        RestoreGlass(dock);
                        RestoreGlass(dock); // Idempotent disable.
                        assert(fill.ReadLocalValue(Shapes::Shape::FillProperty()) == before);
                        assert(stroke.ReadLocalValue(Shapes::Shape::FillProperty()) == DependencyProperty::UnsetValue());
                        assert(!Hosting::ElementCompositionPreview::GetElementChildVisual(background));
                        assert(!dock.material);
                    }
                }
            }
            g_theme = DockTheme::Liquid;
            PrepareGlass(dock);
            auto liquidFill = dock.material.try_as<Media::SolidColorBrush>();
            assert(liquidFill && liquidFill.Color().A == 0);
            RestoreGlass(dock);
            // State-plate geometry needs a hosted tree: VisualTreeHelper::GetParent
            // returns null here, so PrepareStatePlate exits before installing a
            // clip. Those checks live in dock-check/fix-regressions.cpp (island).
            g_theme = DockTheme::Native;
            PrepareGlass(dock);
            assert(!dock.material);
            g_theme = DockTheme::Glass;
            for (auto theme : {ElementTheme::Dark, ElementTheme::Light, ElementTheme::Dark}) {
                background.RequestedTheme(theme);
                PrepareGlass(dock);
                assert(dock.material && dock.materialTheme == theme);
                assert(dock.material.as<Media::AcrylicBrush>().TintColor().R ==
                       (theme == ElementTheme::Dark ? 24 : 250));
            }
            RestoreGlass(dock);
            PrepareGlass(dock);
            Media::SolidColorBrush external(winrt::Windows::UI::Color{255, 10, 20, 30});
            auto externalVisual = visual.Compositor().CreateSpriteVisual();
            fill.Fill(external);
            Hosting::ElementCompositionPreview::SetElementChildVisual(background, externalVisual);
            PrepareGlass(dock); // Ownership loss keeps the external brush and visual.
            assert(!dock.material && dock.materialBlocked);
            assert(fill.Fill() == external);
            assert(Hosting::ElementCompositionPreview::GetElementChildVisual(background) == externalVisual);
            assert(stroke.ReadLocalValue(Shapes::Shape::FillProperty()) == DependencyProperty::UnsetValue());
            Hosting::ElementCompositionPreview::SetElementChildVisual(background, nullptr);
            dock.materialBlocked = false;
            grid.Children().RemoveAtEnd(); // Unsupported tree keeps its material.
            PrepareGlass(dock);
            assert(dock.materialBlocked && !dock.material && fill.Fill() == external);
            Controls::Border plate;
            auto originalPlateClip = plate.Clip();
            Media::RectangleGeometry plateClip;
            plateClip.Rect(PlanVerticalClip(64, 72, 16, 68));
            plate.Clip(plateClip);
            assert(plate.Clip() == plateClip);
            plate.Clip(originalPlateClip);
            assert(plate.Clip() == originalPlateClip);
            puts("PASS: real XAML theme installation/restoration/fallback and hover-plate clipping");
            fflush(stdout);
        }
        manager.Close();
    } catch (winrt::hresult_error const& error) {
        printf("XAML ERROR %08x\n", unsigned(error.code().value));
        return 1;
    }
}
