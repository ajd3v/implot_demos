#define IMGUI_DEFINE_MATH_OPERATORS // For ImVec2 operators

#include <vector>
#include <string>
#include <map>      
#include <chrono>
#include <algorithm> 
#include <numeric>   

// ImGui & ImPlot
#include "imgui.h"
// #include "imgui_internal.h" // Removed as problematic docking features are being removed
#include "implot.h"

// Application Modules
#include "MarketDataGenerator.h" 
#include "MarketSelector.h"
#include "OHLCPlot.h"
#include "VolumePlot.h"
#include "MiniTrendPlot.h"
#include "TechnicalAnalysis.h"
#include "RSIPlot.h"

namespace AppState {
    std::map<std::string, MarketDataGenerator> marketGenerators; 
    std::vector<std::string> marketNames;
    int currentMarketIndex = 0;
    std::string activeMarketName;
    std::vector<MarketDataPoint> activeMarketFullData; 
    std::vector<MarketDataPoint> activeMarketPlotData; 

    bool showSMA = true;
    int smaPeriod = 20;
    std::vector<double> currentSMAValues;
    std::vector<double> currentSMATimestamps;

    bool showRSI = true;
    int rsiPeriod = 14;
    std::vector<double> currentRSIValues;
    std::vector<double> currentRSITimestamps;

    int miniTrendPoints = 60;
    float lastUpdateTime = 0.0f;
    float updateInterval = 1.0f; 
    const int MAX_PLOT_POINTS = 500; 
    const double DATA_GENERATION_INTERVAL_SECONDS = 60.0; 
} 

void updateTAIndicators() {
    using namespace AppState;

    currentSMAValues.clear();
    currentSMATimestamps.clear();
    currentRSIValues.clear();
    currentRSITimestamps.clear();

    if (activeMarketPlotData.empty()) {
        return;
    }

    int validatedSmaPeriod = std::max(1, std::min(smaPeriod, static_cast<int>(activeMarketPlotData.size())));
    int validatedRsiPeriod = std::max(1, std::min(rsiPeriod, static_cast<int>(activeMarketPlotData.size()) > 1 ? static_cast<int>(activeMarketPlotData.size()) -1 : 1));


    if (showSMA && validatedSmaPeriod > 0 && activeMarketPlotData.size() >= static_cast<size_t>(validatedSmaPeriod)) {
        currentSMAValues = TA::CalculateSMA(activeMarketPlotData, validatedSmaPeriod);
        if (!currentSMAValues.empty()) {
            currentSMATimestamps.reserve(currentSMAValues.size());
            for (size_t i = 0; i < currentSMAValues.size(); ++i) {
                if (((validatedSmaPeriod - 1) + i) < activeMarketPlotData.size()) { 
                    currentSMATimestamps.push_back(activeMarketPlotData[(validatedSmaPeriod - 1) + i].Timestamp);
                }
            }
        }
    }
    
    if (showRSI && validatedRsiPeriod > 0 && activeMarketPlotData.size() > static_cast<size_t>(validatedRsiPeriod)) { 
        currentRSIValues = TA::CalculateRSI(activeMarketPlotData, validatedRsiPeriod);
        if (!currentRSIValues.empty()) {
            currentRSITimestamps.reserve(currentRSIValues.size());
            for (size_t i = 0; i < currentRSIValues.size(); ++i) {
                 if ((validatedRsiPeriod + i) < activeMarketPlotData.size()) { 
                    currentRSITimestamps.push_back(activeMarketPlotData[validatedRsiPeriod + i].Timestamp);
                 }
            }
        }
    }
}

void setActiveMarket(const std::string& name) {
    using namespace AppState;
    activeMarketName = name;

    if (marketGenerators.count(activeMarketName)) {
        activeMarketFullData = marketGenerators.at(activeMarketName).getData(); 
        
        activeMarketPlotData.clear();
        int startIdx = std::max(0, static_cast<int>(activeMarketFullData.size()) - MAX_PLOT_POINTS);
        activeMarketPlotData.reserve(activeMarketFullData.size() - startIdx);
        for (size_t i = startIdx; i < activeMarketFullData.size(); ++i) {
            activeMarketPlotData.push_back(activeMarketFullData[i]);
        }
    } else {
        activeMarketFullData.clear();
        activeMarketPlotData.clear();
    }
    updateTAIndicators();
}

void ApplicationMainLoopIteration() {
    using namespace AppState;

    float currentTime = ImGui::GetTime();
    if (lastUpdateTime == 0.0f && currentTime > 0.1f) { // Initialize on first valid time
        lastUpdateTime = currentTime;
    }

    if (currentTime - lastUpdateTime >= updateInterval) {
        lastUpdateTime = currentTime;
        if (marketGenerators.count(activeMarketName)) {
            marketGenerators.at(activeMarketName).generateNewDataPoint(DATA_GENERATION_INTERVAL_SECONDS);
            
            activeMarketFullData = marketGenerators.at(activeMarketName).getData();
            activeMarketPlotData.clear();
            int startIdx = std::max(0, static_cast<int>(activeMarketFullData.size()) - MAX_PLOT_POINTS);
            activeMarketPlotData.reserve(activeMarketFullData.size() - startIdx);
            for (size_t i = startIdx; i < activeMarketFullData.size(); ++i) {
                activeMarketPlotData.push_back(activeMarketFullData[i]);
            }
            updateTAIndicators();
        }
    }

    // --- Simplified Main Window Layout ---
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    // Removed ImGuiWindowFlags_MenuBar as it wasn't used and simplifies the main window further.
    // Removed ImGuiWindowFlags_NoDocking as the main window itself won't be docked.

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); // No padding for the main window hosting children
    ImGui::Begin("MainApplicationWindow", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    // Since ImGui::DockSpace is problematic, use simple child windows side-by-side.
    float leftPanelWidth = 250.0f;
    if (leftPanelWidth > ImGui::GetContentRegionAvail().x * 0.5f) { // Ensure left panel is not too wide
        leftPanelWidth = ImGui::GetContentRegionAvail().x * 0.3f;
    }


    // --- Left Panel (Market Selector & Mini Plot & TA Controls) ---
    ImGui::BeginChild("LeftPanelChild", ImVec2(leftPanelWidth, 0), true); 

    if (ShowMarketSelector(marketNames, currentMarketIndex, ImGui::GetContentRegionAvail().x)) {
        if (static_cast<size_t>(currentMarketIndex) < marketNames.size() && 
            marketNames[currentMarketIndex] != activeMarketName) { 
             setActiveMarket(marketNames[currentMarketIndex]);
        }
    }

    ImGui::Separator();
    if (!activeMarketName.empty()) {
        ImGui::Text("%s Mini Trend:", activeMarketName.c_str());
        ShowMiniTrendPlot("ActiveMiniTrend", activeMarketPlotData, miniTrendPoints, ImVec2(ImGui::GetContentRegionAvail().x, 50));
    }

    ImGui::Separator();
    ImGui::Text("Technical Analysis:");

    bool taSettingsChanged = false; 
    ImGui::Checkbox("Show SMA", &showSMA); ImGui::SameLine(); 
    ImGui::SetNextItemWidth(100);
    if (ImGui::InputInt("SMA Period", &smaPeriod, 1, 5)) {
        smaPeriod = std::max(1, smaPeriod); 
        taSettingsChanged = true;
    }

    ImGui::Checkbox("Show RSI", &showRSI); ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    if (ImGui::InputInt("RSI Period", &rsiPeriod, 1, 5)) {
        rsiPeriod = std::max(1, rsiPeriod); 
        taSettingsChanged = true;
    }
    
    if (taSettingsChanged) {
        updateTAIndicators();
    }
    ImGui::EndChild(); // End LeftPanelChild

    ImGui::SameLine();

    // --- Right Panel (Main Charts) ---
    ImGui::BeginChild("RightPanelChild", ImVec2(0, 0), true); // Use border for clarity

    if (!activeMarketName.empty()) {
        ShowOHLCPlot(activeMarketName, activeMarketPlotData, showSMA, currentSMAValues, currentSMATimestamps, smaPeriod);
        ShowVolumePlot(activeMarketName, activeMarketPlotData); 
        if (showRSI) {
            ShowRSIPlot("ActiveRSI", currentRSIValues, currentRSITimestamps, rsiPeriod); 
        }
    } else {
        ImGui::Text("Select a market to view charts.");
    }
    ImGui::EndChild(); // End RightPanelChild

    ImGui::End(); // End MainApplicationWindow
}


int main(int, char**) {
    // Conceptual: Initialize ImGui, ImPlot, backends, load theme
    // e.g. SetupWindow(); ImGui::CreateContext(); ImPlot::CreateContext(); AppTheme::ApplyProfessionalDarkTheme(); InitBackends();

    using namespace AppState;
    marketGenerators.emplace("BTC/USD", MarketDataGenerator("BTC/USD", 50000.0, 0.005, 100.0));
    marketGenerators.emplace("ETH/USD", MarketDataGenerator("ETH/USD", 4000.0, 0.008, 200.0));
    marketGenerators.emplace("AAPL", MarketDataGenerator("AAPL", 170.0, 0.01, 500.0));

    for (auto const& [key, val_placeholder] : marketGenerators) {
        marketNames.push_back(key);
        marketGenerators.at(key).generateInitialHistory(MAX_PLOT_POINTS + std::max(smaPeriod, rsiPeriod) + 100, DATA_GENERATION_INTERVAL_SECONDS); 
    }

    if (!marketNames.empty()) {
        currentMarketIndex = 0;
        setActiveMarket(marketNames[currentMarketIndex]);
    }
    // lastUpdateTime is initialized in ApplicationMainLoopIteration first run.

    // Conceptual: Main loop
    // while (!WindowShouldClose()) {
    //     PollEvents();
    //     NewFrameBackends();
    //     ImGui::NewFrame(); 
    //     ImPlot::NewFrame();
    //
    //     ApplicationMainLoopIteration(); 
    //
    //     ImGui::Render();
    //     RenderDrawDataBackends();
    //     SwapBuffers();
    // }

    // Conceptual: Cleanup();
    return 0;
}
