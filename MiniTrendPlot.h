#ifndef MINI_TREND_PLOT_H
#define MINI_TREND_PLOT_H

#include <vector>
#include <string>
#include <algorithm> // For std::min

#include "imgui.h"
#include "implot.h"
#include "MarketDataGenerator.h" // Provides MarketDataPoint definition
// PlotUtils.h is not strictly needed here if not using its helpers, but good for consistency
// #include "PlotUtils.h" 

// plotId: A unique ID for the ImPlot instance (e.g., "MiniPlot_" + marketName).
// ohlcData: A vector of MarketDataPoint structs. The function will typically plot the 'Close' prices.
// numRecentPoints: The number of most recent data points from ohlcData to display.
// size: The ImVec2 dimensions for this mini plot.
// Corrected from MarketDataGenerator::MarketDataPoint to MarketDataPoint
inline void ShowMiniTrendPlot(const std::string& plotId, const std::vector<MarketDataPoint>& ohlcData, int numRecentPoints, ImVec2 size) {
    if (ohlcData.empty() || numRecentPoints <= 0) {
        // Render an empty plot canvas if no data
    }

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
            const auto& dataPoint = ohlcData[startIndex + i]; // dataPoint is MarketDataPoint
            xs.push_back(dataPoint.Timestamp); 
            ys.push_back(dataPoint.Close);
        }
    }

    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0, 0));
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotBorderSize, 0); 
    ImPlot::PushStyleVar(ImPlotStyleVar_FitPadding, ImVec2(0,0)); 

    if (ImPlot::BeginPlot(plotId.c_str(), size, ImPlotFlags_CanvasOnly)) {
        ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_NoDecorations | ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_NoDecorations | ImPlotAxisFlags_AutoFit);
        
        if (pointsToPlot > 0) {
            ImPlot::PlotLine("##MiniLine", xs.data(), ys.data(), pointsToPlot);
        }
        
        ImPlot::EndPlot();
    }
    ImPlot::PopStyleVar(3); 
}

#endif // MINI_TREND_PLOT_H
