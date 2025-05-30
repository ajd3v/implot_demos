#ifndef THEME_H
#define THEME_H

#include "imgui.h"
#include "implot.h"

namespace AppTheme {

// Helper to convert hex to ImVec4 (RGB only, Alpha is 1.0f)
inline ImVec4 HexToImVec4(const char* hex) {
    unsigned int r, g, b;
    sscanf(hex, "#%02x%02x%02x", &r, &g, &b);
    return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
}

// Helper to convert hex to ImVec4 (RGBA)
inline ImVec4 HexToImVec4Alpha(const char* hex, float alpha) {
    ImVec4 color = HexToImVec4(hex);
    color.w = alpha;
    return color;
}


void ApplyProfessionalDarkTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImPlotStyle& implotStyle = ImPlot::GetStyle();

    // --- Color Palette ---
    // Backgrounds
    ImVec4 bgColor          = HexToImVec4("#1E1E1E"); // Main background
    ImVec4 bgLighter1       = HexToImVec4("#252526"); // Slightly lighter bg (window, inputs)
    ImVec4 bgLighter2       = HexToImVec4("#2D2D2D"); // Even lighter (headers, hovered items)
    ImVec4 bgDarker1        = HexToImVec4("#1A1A1A"); // Slightly darker for contrast if needed

    // Text
    ImVec4 textColor        = HexToImVec4("#D4D4D4"); // Main text
    ImVec4 textDisabled     = HexToImVec4("#7A7A7A"); // Disabled text

    // Accents & Controls
    ImVec4 accentPrimary    = HexToImVec4("#0A84FF"); // Muted blue (e.g., selections, active items)
    ImVec4 accentSecondary  = HexToImVec4("#30D158"); // Muted green (e.g., positive indicators)
    ImVec4 accentNegative   = HexToImVec4("#FF453A"); // Muted red (e.g., negative indicators)
    ImVec4 controlBorder    = HexToImVec4("#3C3C3C"); // Subtle borders for inputs/frames

    // --- ImGui Style Settings ---
    style.WindowPadding     = ImVec2(8, 8);
    style.FramePadding      = ImVec2(6, 4);
    style.ItemSpacing       = ImVec2(8, 4);
    style.ItemInnerSpacing  = ImVec2(4, 4);
    style.ScrollbarSize     = 14.0f;
    style.GrabMinSize       = 12.0f;

    style.WindowRounding    = 4.0f;
    style.ChildRounding     = 4.0f;
    style.FrameRounding     = 4.0f;
    style.PopupRounding     = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding      = 4.0f;
    style.TabRounding       = 4.0f;

    style.WindowBorderSize  = 1.0f;
    style.FrameBorderSize   = 1.0f;
    style.PopupBorderSize   = 1.0f;
    style.ChildBorderSize   = 1.0f; // Keep child borders consistent or remove if preferred

    // Colors
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]                   = textColor;
    colors[ImGuiCol_TextDisabled]           = textDisabled;
    colors[ImGuiCol_WindowBg]               = bgLighter1; // Main window background
    colors[ImGuiCol_ChildBg]                = bgColor;    // Child window background (e.g. market selector)
    colors[ImGuiCol_PopupBg]                = bgLighter1;
    colors[ImGuiCol_Border]                 = controlBorder;
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); // No shadow

    colors[ImGuiCol_FrameBg]                = bgLighter2; // Input fields
    colors[ImGuiCol_FrameBgHovered]         = HexToImVec4Alpha("#FFFFFF", 0.1f);
    colors[ImGuiCol_FrameBgActive]          = HexToImVec4Alpha("#FFFFFF", 0.15f);

    colors[ImGuiCol_TitleBg]                = bgColor; // Title bar of windows
    colors[ImGuiCol_TitleBgActive]          = accentPrimary;
    colors[ImGuiCol_TitleBgCollapsed]       = bgColor;
    colors[ImGuiCol_MenuBarBg]              = bgLighter1;

    colors[ImGuiCol_ScrollbarBg]            = bgColor;
    colors[ImGuiCol_ScrollbarGrab]          = bgLighter2;
    colors[ImGuiCol_ScrollbarGrabHovered]   = HexToImVec4Alpha(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.7f);
    colors[ImGuiCol_ScrollbarGrabActive]    = accentPrimary;

    colors[ImGuiCol_CheckMark]              = accentPrimary;
    colors[ImGuiCol_SliderGrab]             = accentPrimary;
    colors[ImGuiCol_SliderGrabActive]       = HexToImVec4Alpha(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.7f);

    colors[ImGuiCol_Button]                 = bgLighter2;
    colors[ImGuiCol_ButtonHovered]          = HexToImVec4Alpha(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.4f);
    colors[ImGuiCol_ButtonActive]           = accentPrimary;

    colors[ImGuiCol_Header]                 = bgLighter2; // Headers for collapsing sections etc.
    colors[ImGuiCol_HeaderHovered]          = HexToImVec4Alpha(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.4f);
    colors[ImGuiCol_HeaderActive]           = accentPrimary;

    colors[ImGuiCol_Separator]              = controlBorder;
    colors[ImGuiCol_SeparatorHovered]       = accentPrimary;
    colors[ImGuiCol_SeparatorActive]        = HexToImVec4Alpha(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.7f);

    colors[ImGuiCol_ResizeGrip]             = bgLighter2;
    colors[ImGuiCol_ResizeGripHovered]      = HexToImVec4Alpha(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.4f);
    colors[ImGuiCol_ResizeGripActive]       = accentPrimary;

    colors[ImGuiCol_Tab]                    = bgLighter2;
    colors[ImGuiCol_TabHovered]             = HexToImVec4Alpha(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.4f);
    colors[ImGuiCol_TabActive]              = accentPrimary;
    colors[ImGuiCol_TabUnfocused]           = bgLighter2;
    colors[ImGuiCol_TabUnfocusedActive]     = bgLighter1; // When tab is active but window not focused

    colors[ImGuiCol_DockingPreview]         = HexToImVec4Alpha(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.7f);
    colors[ImGuiCol_DockingEmptyBg]         = HexToImVec4(0.12f, 0.12f, 0.12f, 1.00f);

    colors[ImGuiCol_PlotLines]              = accentSecondary; // Default line color for plots
    colors[ImGuiCol_PlotLinesHovered]       = accentPrimary;
    colors[ImGuiCol_PlotHistogram]          = accentSecondary;
    colors[ImGuiCol_PlotHistogramHovered]   = accentPrimary;

    colors[ImGuiCol_TableHeaderBg]          = bgLighter2;
    colors[ImGuiCol_TableBorderStrong]      = controlBorder;
    colors[ImGuiCol_TableBorderLight]       = HexToImVec4Alpha(controlBorder.x, controlBorder.y, controlBorder.z, 0.5f);
    colors[ImGuiCol_TableRowBg]             = bgColor;
    colors[ImGuiCol_TableRowBgAlt]          = bgLighter1;

    colors[ImGuiCol_TextSelectedBg]         = HexToImVec4Alpha(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = HexToImVec4Alpha(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.9f);
    colors[ImGuiCol_NavHighlight]           = HexToImVec4Alpha(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.8f);
    colors[ImGuiCol_NavWindowingHighlight]  = HexToImVec4Alpha(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.7f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);


    // --- ImPlot Style Settings ---
    implotStyle.UseLocalTime = true; // Display time in local timezone
    implotStyle.Colormap = ImPlotColormap_Dark; // Base colormap

    // Plot specific colors (many are set locally during plotting for more control)
    implotStyle.Colors[ImPlotCol_PlotBg]    = HexToImVec4Alpha(bgColor.x, bgColor.y, bgColor.z, 0.9f); // Plot background slightly transparent or opaque
    implotStyle.Colors[ImPlotCol_FrameBg]   = HexToImVec4Alpha(bgColor.x, bgColor.y, bgColor.z, 0.9f); // Frame around plot
    implotStyle.Colors[ImPlotCol_GridLines] = HexToImVec4Alpha(controlBorder.x, controlBorder.y, controlBorder.z, 0.6f); // Faint grid lines
    implotStyle.Colors[ImPlotCol_AxisText]  = textColor;
    implotStyle.Colors[ImPlotCol_AxisTick]  = textColor;
    implotStyle.Colors[ImPlotCol_AxisBg]    = ImVec4(0,0,0,0); // Transparent axis background

    implotStyle.Colors[ImPlotCol_Title]     = textColor;
    implotStyle.Colors[ImPlotCol_LegendBg]  = bgLighter1;
    implotStyle.Colors[ImPlotCol_LegendBorder] = controlBorder;
    implotStyle.Colors[ImPlotCol_LegendText]= textColor;

    implotStyle.Colors[ImPlotCol_Selection] = HexToImVec4Alpha(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.4f); // Box select color
    implotStyle.Colors[ImPlotCol_Query]     = HexToImVec4Alpha(accentPrimary.x, accentPrimary.y, accentPrimary.z, 0.6f); // Query/crosshair color

    // Default line colors for plots if not overridden locally
    // These can be used as defaults for SMA, RSI lines etc.
    implotStyle.Colors[ImPlotCol_Line]      = accentSecondary; // Default line color (e.g. green)
    // ImPlotCol_Fill is often used for bars, area under lines
    implotStyle.Colors[ImPlotCol_Fill]      = HexToImVec4Alpha(accentSecondary.x, accentSecondary.y, accentSecondary.z, 0.3f); 

    // Markers
    implotStyle.Colors[ImPlotCol_MarkerFill] = accentSecondary;
    implotStyle.Colors[ImPlotCol_MarkerOutline] = HexToImVec4(0,0,0,0); // No outline for markers by default

    // Error Bars
    implotStyle.Colors[ImPlotCol_ErrorBar] = textColor;


    // Custom Colors for specific plot items (often set locally in plotting functions)
    // These are more like placeholders or defaults if a specific plot doesn't set its own.
    // Example: If you have a global "VolumeBarColor", "SMALineColor" etc.
    // For Candlesticks, colors are typically passed directly to PlotCandlesticks.
    // ImPlot does not have global ImPlotCol_CandleBullFill, ImPlotCol_CandleBearFill.
    // These need to be defined in your application and passed to the ShowOHLCPlot function,
    // or ShowOHLCPlot itself defines them. The theme can define *suggested* colors.
    // AppTheme::BullColor = HexToImVec4("#26A69A");
    // AppTheme::BearColor = HexToImVec4("#EF5350");
    // AppTheme::VolumeColor = HexToImVec4("#2962FF"); // Example blue
    // AppTheme::SMALineColor = HexToImVec4("#FFCA28"); // Example amber/yellow
    // AppTheme::RSILineColor = HexToImVec4("#AB47BC"); // Example purple
    // AppTheme::RSILineInflectionColor = HexToImVec4("#B0BEC5"); // Lighter gray for RSI 30/70 lines

    // Plot styling variables
    implotStyle.PlotDefaultSize = ImVec2(-1, 300); // Default plot height if not specified
    implotStyle.PlotMinSize     = ImVec2(200, 150);
    implotStyle.PlotPadding     = ImVec2(0.05f * ImGui::GetFontSize(), 0.05f * ImGui::GetFontSize()); // Relative padding
    implotStyle.AnnotationPadding = ImVec2(2,2);
    implotStyle.FitPadding      = ImVec2(0.1f, 0.1f); // Padding for auto-fitting axes
    implotStyle.DefaultColors   = 10; // Number of default line colors in the cycle

    implotStyle.AntiAliasedLines = true;
}

} // namespace AppTheme

#endif // THEME_H
