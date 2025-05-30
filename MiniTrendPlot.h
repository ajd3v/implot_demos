#ifndef MINI_TREND_PLOT_H
#define MINI_TREND_PLOT_H

#include <vector>
#include <string>
#include <algorithm> // For std::min

#include "imgui.h"
#include "implot.h"
#include "MarketDataGenerator.h" // For MarketDataPoint struct

// plotId: A unique ID for the ImPlot instance (e.g., "MiniPlot_" + marketName).
// ohlcData: A vector of MarketDataPoint structs. The function will typically plot the 'Close' prices.
// numRecentPoints: The number of most recent data points from ohlcData to display.
// size: The ImVec2 dimensions for this mini plot.
inline void ShowMiniTrendPlot(const std::string& plotId, const std::vector<MarketDataGenerator::MarketDataPoint>& ohlcData, int numRecentPoints, ImVec2 size) {
    if (ohlcData.empty() || numRecentPoints <= 0) {
        // If there's no data or no points to show, one might render an empty space of 'size'
        // or simply return. For an actual empty plot, ImGui::Dummy(size) could be used
        // outside this function if needed. Here, we'll just let ImPlot render an empty canvas.
    }

    // Determine the starting point in ohlcData
    int totalPoints = static_cast<int>(ohlcData.size());
    int pointsToPlot = std::min(numRecentPoints, totalPoints);
    int startIndex = totalPoints - pointsToPlot;
    if (startIndex < 0) startIndex = 0;

    std::vector<double> xs;
    std::vector<double> ys;
    xs.reserve(pointsToPlot);
    ys.reserve(pointsToPlot);

    if (pointsToPlot > 0) {
        for (int i = 0; i < pointsToPlot; ++i) {
            const auto& dataPoint = ohlcData[startIndex + i];
            xs.push_back(dataPoint.Timestamp); // Or use simple indices if time scale isn't critical for mini plot
            ys.push_back(dataPoint.Close);
        }
    }

    // Style for compactness
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0, 0));
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotBorderSize, 0); // No border for the plot itself
    ImPlot::PushStyleVar(ImPlotStyleVar_FitPadding, ImVec2(0,0)); // No padding for fitting data

    if (ImPlot::BeginPlot(plotId.c_str(), size, ImPlotFlags_CanvasOnly)) {
        // Setup axes for minimalist appearance, ensuring data fits but no decorations
        ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_NoDecorations | ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_NoDecorations | ImPlotAxisFlags_AutoFit);
        
        if (pointsToPlot > 0) {
            ImPlot::PlotLine("##MiniLine", xs.data(), ys.data(), pointsToPlot);
        }
        
        ImPlot::EndPlot();
    }
    ImPlot::PopStyleVar(3); // Pop PlotPadding, PlotBorderSize, FitPadding
}

#endif // MINI_TREND_PLOT_H
