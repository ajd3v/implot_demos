#ifndef RSI_PLOT_H
#define RSI_PLOT_H

#include <vector>
#include <string>
#include "imgui.h"
#include "implot.h"
#include "PlotUtils.h" // For FormatTimestampForTooltip and FindClosestPointIndex

// plotId: Unique ID for the ImPlot (e.g., "RSI_" + marketName).
// rsiValues: Data from CalculateRSI.
// rsiTimestamps: Corresponding timestamps for rsiValues.
// rsiPeriod: The RSI period, used for labeling in tooltip.
// plotHeight: Height of the RSI plot.
inline void ShowRSIPlot(const std::string& plotId,
                        const std::vector<double>& rsiValues,
                        const std::vector<double>& rsiTimestamps,
                        int rsiPeriod, 
                        float plotHeight = 100.0f) {

    if (ImPlot::BeginPlot(plotId.c_str(), ImVec2(-1, plotHeight))) {
        ImPlot::LinkNextPlotAxesX();

        ImPlot::SetupAxis(ImAxis_X1, "Time", ImPlot::ImPlotAxisFlags_Time); // Corrected
        ImPlot::SetupAxis(ImAxis_Y1, "RSI", ImPlot::ImPlotAxisFlags_LockMin | ImPlot::ImPlotAxisFlags_LockMax); // Corrected
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImPlot::ImPlotCond_Always); // Corrected

        if (!rsiValues.empty() && !rsiTimestamps.empty() && rsiValues.size() == rsiTimestamps.size()) {
            ImPlot::PlotLine("##RSI", rsiTimestamps.data(), rsiValues.data(), static_cast<int>(rsiValues.size()));

            double p_levels[] = {30.0, 70.0};
            ImPlot::PushStyleColor(ImPlot::ImPlotCol_Line, ImVec4(0.7f, 0.7f, 0.7f, 0.8f)); // Corrected
            ImPlot::PlotInfLines("##Levels", p_levels, 2, ImPlot::ImPlotInfLinesFlags_Horizontal); // Corrected
            ImPlot::PopStyleColor();

            // Tooltip Logic
            if (ImPlot::IsPlotHovered()) {
                ImPlot::ImPlotPoint mouse = ImPlot::GetPlotMousePos(); // Corrected
                int hoveredIndex = FindClosestPointIndex(rsiTimestamps, mouse.x);

                if (hoveredIndex != -1) {
                    // Ensure index is valid for rsiValues as well
                    if (static_cast<size_t>(hoveredIndex) < rsiValues.size()) { 
                        ImGui::BeginTooltip();
                        ImGui::Text("Time: %s", FormatTimestampForTooltip(rsiTimestamps[hoveredIndex]).c_str());
                        ImGui::Separator();
                        ImGui::Text("RSI(%d): %.2f", rsiPeriod, rsiValues[hoveredIndex]);
                        ImGui::EndTooltip();
                    }
                }
            }
        } else {
            // Handle empty or mismatched data
        }
        ImPlot::EndPlot();
    }
}

#endif // RSI_PLOT_H
