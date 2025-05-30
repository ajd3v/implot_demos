#ifndef OHLC_PLOT_H
#define OHLC_PLOT_H

#include <vector>
#include <string>
#include <numeric> 
#include <algorithm> 
#include <cstdio> // For sprintf (alternative to std::to_string for formatting)

#include "imgui.h"
#include "implot.h"
#include "MarketDataGenerator.h" // Provides MarketDataPoint definition
#include "PlotUtils.h"           // For FormatTimestampForTooltip and FindClosestPointIndex

// marketName: Name of the market to display as the plot title.
// ohlcData: A vector of MarketDataPoint structs containing the OHLC data.
// showSMA: Toggle to display the SMA line.
// smaValues: Vector of SMA values.
// smaTimestamps: Vector of timestamps corresponding to each SMA value.
// smaPeriod: The period of the SMA (for tooltip label).
inline void ShowOHLCPlot(const std::string& marketName,
                         const std::vector<MarketDataPoint>& ohlcData,
                         bool showSMA,
                         const std::vector<double>& smaValues,
                         const std::vector<double>& smaTimestamps,
                         int smaPeriod) {
    std::string plotTitle = "OHLC: " + marketName;

    if (ImPlot::BeginPlot(plotTitle.c_str(), ImVec2(-1, 0))) { 
        if (ohlcData.empty()) {
            // Handle empty data
        } else {
            std::vector<double> xs(ohlcData.size());
            std::vector<double> opens(ohlcData.size());
            std::vector<double> closes(ohlcData.size());
            std::vector<double> lows(ohlcData.size());
            std::vector<double> highs(ohlcData.size());

            for (size_t i = 0; i < ohlcData.size(); ++i) {
                xs[i] = ohlcData[i].Timestamp;
                opens[i] = ohlcData[i].Open;
                closes[i] = ohlcData[i].Close;
                lows[i] = ohlcData[i].Low;
                highs[i] = ohlcData[i].High;
            }

            // Corrected: Removed ImPlot:: prefix for specific flags
            ImPlot::SetupAxis(ImAxis_X1, "Time", ImPlotAxisFlags_Time); 
            ImPlot::SetupAxis(ImAxis_Y1, "Price", ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_LockMin); 

            // Corrected: Removed ImPlot:: prefix for PlotCandlesticks
            PlotCandlesticks("##Candlesticks", // Assumed global or `using namespace ImPlot;` is somewhere
                                     xs.data(), opens.data(), closes.data(), lows.data(), highs.data(),
                                     static_cast<int>(ohlcData.size()), 0.2f);

            if (showSMA && !smaValues.empty() && !smaTimestamps.empty() && smaValues.size() == smaTimestamps.size()) {
                // Corrected: Removed ImPlot:: prefix for ImPlotCol_Line
                ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0f, 0.0f, 1.0f, 1.0f)); 
                ImPlot::PlotLine("##SMA", smaTimestamps.data(), smaValues.data(), static_cast<int>(smaValues.size()));
                ImPlot::PopStyleColor();
            }

            // Tooltip Logic
            if (ImPlot::IsPlotHovered() && !ohlcData.empty()) {
                // Corrected: Removed ImPlot:: prefix for ImPlotPoint
                ImPlotPoint mouse = ImPlot::GetPlotMousePos(); 
                int ohlcIndex = FindClosestPointIndex(xs, mouse.x); 

                if (ohlcIndex != -1) {
                    const auto& dp = ohlcData[ohlcIndex]; 
                    ImGui::BeginTooltip();
                    ImGui::Text("Time: %s", FormatTimestampForTooltip(dp.Timestamp).c_str());
                    ImGui::Separator();
                    ImGui::Text("Open:   %.2f", dp.Open);
                    ImGui::Text("High:   %.2f", dp.High);
                    ImGui::Text("Low:    %.2f", dp.Low);
                    ImGui::Text("Close:  %.2f", dp.Close);

                    if (showSMA && !smaValues.empty() && !smaTimestamps.empty()) {
                        int smaTooltipIndex = FindClosestPointIndex(smaTimestamps, mouse.x);
                        if (smaTooltipIndex != -1) {
                             if (static_cast<size_t>(smaTooltipIndex) < smaValues.size() && static_cast<size_t>(smaTooltipIndex) < smaTimestamps.size() &&
                                 std::abs(smaTimestamps[smaTooltipIndex] - xs[ohlcIndex]) < ( (xs.size() > 1 && ohlcIndex > 0 && ohlcIndex < xs.size()) ? (xs[ohlcIndex] - xs[ohlcIndex-1])/2.0 : (xs.size() > 1 && ohlcIndex == 0 ? (xs[1]-xs[0])/2.0 : 30.0) ) ) {
                                ImGui::Text("SMA(%d): %.2f", smaPeriod, smaValues[smaTooltipIndex]);
                             }
                        }
                    }
                    ImGui::EndTooltip();
                }
            }
        }
        ImPlot::EndPlot();
    }
}

#endif // OHLC_PLOT_H
