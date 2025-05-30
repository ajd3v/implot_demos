#define IMGUI_DEFINE_MATH_OPERATORS // For ImVec2 operators

#include <vector>
#include <string>
#include <map>      // Included for std::map
#include <chrono>
#include <algorithm> // For std::min, std::max
#include <numeric>   // For std::iota (potentially, if needed elsewhere)

// ImGui & ImPlot
#include "imgui.h"
#include "imgui_internal.h" // Included for ImGuiDockNodeFlags_PassthruCentralNode and ImFormatString
#include "implot.h"

// Application Modules
#include "MarketDataGenerator.h" // Provides MarketDataPoint and MarketDataGenerator class
#include "MarketSelector.h"
#include "OHLCPlot.h"
#include "VolumePlot.h"
#include "MiniTrendPlot.h"
#include "TechnicalAnalysis.h"
#include "RSIPlot.h"
// PlotUtils.h is included by the plot headers that use it.

// --- Global/Static Application State (Simplified) ---
namespace AppState {
    // Corrected definition: MarketDataGenerator (class) not MarketDataGenerator::MarketDataGenerator
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
    float updateInterval = 1.0f; // Seconds
    const int MAX_PLOT_POINTS = 500; 
    const double DATA_GENERATION_INTERVAL_SECONDS = 60.0; 
} // namespace AppState

// --- Helper Function: Update TA Indicators ---
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
    // For RSI, period must be less than data size, as it uses N+1 points for N changes.
    int validatedRsiPeriod = std::max(1, std::min(rsiPeriod, static_cast<int>(activeMarketPlotData.size()) > 1 ? static_cast<int>(activeMarketPlotData.size()) -1 : 1));


    if (showSMA && validatedSmaPeriod > 0 && activeMarketPlotData.size() >= static_cast<size_t>(validatedSmaPeriod)) {
        currentSMAValues = TA::CalculateSMA(activeMarketPlotData, validatedSmaPeriod);
        if (!currentSMAValues.empty()) {
            currentSMATimestamps.reserve(currentSMAValues.size());
            for (size_t i = 0; i < currentSMAValues.size(); ++i) {
                if (((validatedSmaPeriod - 1) + i) < activeMarketPlotData.size()) { // Boundary check
                    currentSMATimestamps.push_back(activeMarketPlotData[(validatedSmaPeriod - 1) + i].Timestamp);
                }
            }
        }
    }
    
    // RSI calculation needs at least 'period + 1' data points to calculate 'period' changes.
    // TA::CalculateRSI itself checks if points.size() <= period.
    if (showRSI && validatedRsiPeriod > 0 && activeMarketPlotData.size() > static_cast<size_t>(validatedRsiPeriod)) { 
        currentRSIValues = TA::CalculateRSI(activeMarketPlotData, validatedRsiPeriod);
        if (!currentRSIValues.empty()) {
            currentRSITimestamps.reserve(currentRSIValues.size());
            for (size_t i = 0; i < currentRSIValues.size(); ++i) {
                 if ((validatedRsiPeriod + i) < activeMarketPlotData.size()) { // Boundary check
                    currentRSITimestamps.push_back(activeMarketPlotData[validatedRsiPeriod + i].Timestamp);
                 }
            }
        }
    }
}

// --- Helper Function: Set Active Market ---
void setActiveMarket(const std::string& name) {
    using namespace AppState;
    activeMarketName = name;

    if (marketGenerators.count(activeMarketName)) {
        activeMarketFullData = marketGenerators.at(activeMarketName).getData(); // Use .at() for const correctness if map is const
        
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
    if (lastUpdateTime == 0.0f) lastUpdateTime = currentTime; // Initialize on first frame if not done in main

    if (currentTime - lastUpdateTime >= updateInterval) {
        lastUpdateTime = currentTime;
        if (marketGenerators.count(activeMarketName)) {
            marketGenerators.at(activeMarketName).generateNewDataPoint(DATA_GENERATION_INTERVAL_SECONDS);
            
            // Update data for the active market
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

    // --- ImGui Dockspace Setup ---
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags host_window_flags = 0;
    host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    host_window_flags |= ImGuiWindowFlags_NoDocking; 

    char label[32];
    ImFormatString(label, IM_ARRAYSIZE(label), "DockSpaceViewport_%08X", viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin(label, nullptr, host_window_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode); 
    ImGui::End();


    // --- Left Panel (Market Selector & Mini Plot & TA Controls) ---
    ImGui::Begin("Controls##LeftPanel"); 

    if (ShowMarketSelector(marketNames, currentMarketIndex, ImGui::GetContentRegionAvail().x)) {
        if (static_cast<size_t>(currentMarketIndex) < marketNames.size() && 
            marketNames[currentMarketIndex] != activeMarketName) { // Bounds check
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

    ImGui::End(); 

    // --- Right Panel (Main Charts) ---
    ImGui::Begin("Charts##RightPanel");

    if (!activeMarketName.empty()) {
        // Corrected ShowOHLCPlot call with smaPeriod
        ShowOHLCPlot(activeMarketName, activeMarketPlotData, showSMA, currentSMAValues, currentSMATimestamps, smaPeriod);
        ShowVolumePlot(activeMarketName, activeMarketPlotData); 
        if (showRSI) {
            ShowRSIPlot("ActiveRSI", currentRSIValues, currentRSITimestamps, rsiPeriod); 
        }
    } else {
        ImGui::Text("Select a market to view charts.");
    }

    ImGui::End(); 
}


// Example main() function (conceptual)
int main(int, char**) {
    // --- Initialize Windowing Library (e.g., GLFW) ---
    // --- Initialize ImGui Context ---
    // --- Initialize ImPlot Context ---
    // --- Initialize ImGui Backend (e.g., GLFW + OpenGL3) ---
    // --- Load Fonts, Styles (e.g. AppTheme::ApplyProfessionalDarkTheme(); ) ---
    
    // Example: Call Theme function after ImGui::CreateContext() and ImPlot::CreateContext()
    // ImGui::CreateContext();
    // ImPlot::CreateContext();
    // AppTheme::ApplyProfessionalDarkTheme(); // Assuming Theme.h is included and defines this
    // ImGui_ImplGlfw_Init...
    // ImGui_ImplOpenGL3_Init...


    // Application Initialization
    using namespace AppState;
    // Corrected emplace calls
    marketGenerators.emplace("BTC/USD", MarketDataGenerator("BTC/USD", 50000.0, 0.005, 100.0));
    marketGenerators.emplace("ETH/USD", MarketDataGenerator("ETH/USD", 4000.0, 0.008, 200.0));
    marketGenerators.emplace("AAPL", MarketDataGenerator("AAPL", 170.0, 0.01, 500.0));

    for (auto const& [key, val_placeholder] : marketGenerators) { // val_placeholder is not used directly due to const
        marketNames.push_back(key);
        // Access generator by key to call non-const method
        marketGenerators.at(key).generateInitialHistory(MAX_PLOT_POINTS + std::max(smaPeriod, rsiPeriod) + 100, DATA_GENERATION_INTERVAL_SECONDS); 
    }


    if (!marketNames.empty()) {
        currentMarketIndex = 0;
        setActiveMarket(marketNames[currentMarketIndex]);
    }
    // lastUpdateTime should be initialized after ImGui::GetTime() is available, typically in the main loop first iteration or after init.
    // AppState::lastUpdateTime = ImGui::GetTime(); // Moved to be handled in ApplicationMainLoopIteration first run

    // --- Main application loop (conceptual, replace with actual backend loop) ---
    // while (true) { // Replace with actual window loop condition
    //     // Backend new frame calls (e.g. glfwPollEvents(), ImGui_ImplOpenGL3_NewFrame(), ImGui_ImplGlfw_NewFrame())
    //     ImGui::NewFrame(); 
    //     ImPlot::NewFrame(); 
    //
    //     ApplicationMainLoopIteration(); 
    //
    //     ImGui::Render(); 
    //     // Backend render call (e.g. ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()))
    //     // Swap buffers
    // }

    // --- Cleanup ImPlot, ImGui, Windowing library ---
    // ImPlot::DestroyContext();
    // ImGui::DestroyContext();
    // Backend shutdown calls
    // Terminate windowing library
    return 0;
}
