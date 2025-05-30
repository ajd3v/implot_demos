#ifndef VOLUME_PLOT_H
#define VOLUME_PLOT_H

#include <vector>
#include <string>
#include <numeric> 
#include <algorithm> 

#include "imgui.h"
#include "implot.h"
#include "MarketDataGenerator.h" // Provides MarketDataPoint definition
#include "PlotUtils.h"           // For FormatTimestampForTooltip and FindClosestPointIndex

// marketName: Name of the market (used for context, possibly in plot item ID if not in title).
// ohlcData: A vector of MarketDataPoint structs containing the timestamp and volume data.
// plotHeight: Suggested height for the volume plot.
inline void ShowVolumePlot(const std::string& marketName,
                           const std::vector<MarketDataPoint>& ohlcData,
                           float plotHeight = 100.0f) {
    std::string plotTitle = "Volume##" + marketName; 

    if (ImPlot::BeginPlot(plotTitle.c_str(), ImVec2(-1, plotHeight))) {
        if (ohlcData.empty()) {
            // Handle empty data
        } else {
            std::vector<double> xs(ohlcData.size());
            std::vector<double> volumes(ohlcData.size());

            for (size_t i = 0; i < ohlcData.size(); ++i) {
                xs[i] = ohlcData[i].Timestamp;
                volumes[i] = ohlcData[i].Volume;
            }
            
            // Corrected: Removed ImPlot:: prefix for LinkNextPlotAxesX
            LinkNextPlotAxesX(); 
            
            // Corrected: Removed ImPlot:: prefix for specific flags
            ImPlot::SetupAxis(ImAxis_X1, "Time", ImPlotAxisFlags_Time); 
            ImPlot::SetupAxis(ImAxis_Y1, "Volume", ImPlotAxisFlags_AutoFit); 
            
            // Corrected: Removed ImPlot:: prefix for ImPlotCol_Fill
            ImPlot::PushStyleColor(ImPlotCol_Fill, ImVec4(0.4f, 0.4f, 0.8f, 0.6f)); 
            
            double barWidthSetting;
            if (ohlcData.size() >= 2) {
                double timeStep = xs[1] - xs[0];
                if (timeStep > 0) {
                    barWidthSetting = timeStep * 0.5; 
                } else {
                    barWidthSetting = 60.0 * 0.5; 
                }
            } else if (ohlcData.size() == 1) {
                barWidthSetting = 30.0; 
            } else {
                barWidthSetting = 1.0;
            }

            ImPlot::PlotBars("##VolumeBars", xs.data(), volumes.data(), static_cast<int>(ohlcData.size()), barWidthSetting);
            ImPlot::PopStyleColor();

            // Tooltip Logic
            if (ImPlot::IsPlotHovered() && !ohlcData.empty()) {
                // Corrected: Removed ImPlot:: prefix for ImPlotPoint
                ImPlotPoint mouse = ImPlot::GetPlotMousePos(); 
                int hoveredIndex = FindClosestPointIndex(ohlcData, mouse.x);

                if (hoveredIndex != -1) {
                    const auto& dp = ohlcData[hoveredIndex]; 
                    ImGui::BeginTooltip();
                    ImGui::Text("Time: %s", FormatTimestampForTooltip(dp.Timestamp).c_str());
                    ImGui::Separator();
                    ImGui::Text("Volume: %.0f", dp.Volume); 
                    ImGui::EndTooltip();
                }
            }
        }
        ImPlot::EndPlot();
    }
}

#endif // VOLUME_PLOT_H
