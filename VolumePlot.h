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
            
            ImPlot::LinkNextPlotAxesX();
            
            ImPlot::SetupAxis(ImAxis_X1, "Time", ImPlot::ImPlotAxisFlags_Time); // Corrected
            ImPlot::SetupAxis(ImAxis_Y1, "Volume", ImPlot::ImPlotAxisFlags_AutoFit); // Corrected
            
            ImPlot::PushStyleColor(ImPlot::ImPlotCol_Fill, ImVec4(0.4f, 0.4f, 0.8f, 0.6f)); // Corrected
            
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
                ImPlot::ImPlotPoint mouse = ImPlot::GetPlotMousePos(); // Corrected
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
