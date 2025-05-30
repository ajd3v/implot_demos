#define IMGUI_DEFINE_MATH_OPERATORS // For ImVec2 operators

#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <algorithm> // For std::min, std::max
#include <numeric>   // For std::iota (potentially, if needed elsewhere)

// ImGui & ImPlot
#include "imgui.h"
#include "implot.h"

// Application Modules (assuming they are in the same directory or include path is set)
#include "MarketDataGenerator.h"
#include "MarketSelector.h"
#include "OHLCPlot.h"
#include "VolumePlot.h"
#include "MiniTrendPlot.h"
#include "TechnicalAnalysis.h"
#include "RSIPlot.h"

// --- Global/Static Application State (Simplified) ---
namespace AppState {
    std::map<std::string, MarketDataGenerator::MarketDataGenerator> marketGenerators;
    std::vector<std::string> marketNames;
    int currentMarketIndex = 0;
    std::string activeMarketName;
    std::vector<MarketDataGenerator::MarketDataPoint> activeMarketFullData; // Holds all data from generator
    std::vector<MarketDataGenerator::MarketDataPoint> activeMarketPlotData; // Subset for plotting (last MAX_PLOT_POINTS)

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
    const int MAX_PLOT_POINTS = 500; // Max points to display in charts
    const double DATA_GENERATION_INTERVAL_SECONDS = 60.0; // New data point every 60s
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

    // Validate periods against available data for plotting
    // Ensure period is at least 1 and not greater than available data points
    int validatedSmaPeriod = std::max(1, std::min(smaPeriod, static_cast<int>(activeMarketPlotData.size())));
    int validatedRsiPeriod = std::max(1, std::min(rsiPeriod, static_cast<int>(activeMarketPlotData.size())));


    if (showSMA && validatedSmaPeriod > 0 && activeMarketPlotData.size() >= static_cast<size_t>(validatedSmaPeriod)) {
        currentSMAValues = TA::CalculateSMA(activeMarketPlotData, validatedSmaPeriod);
        if (!currentSMAValues.empty()) {
            currentSMATimestamps.reserve(currentSMAValues.size());
            for (size_t i = 0; i < currentSMAValues.size(); ++i) {
                // SMA value at index i corresponds to original data point at index (period - 1) + i
                currentSMATimestamps.push_back(activeMarketPlotData[(validatedSmaPeriod - 1) + i].Timestamp);
            }
        }
    }

    if (showRSI && validatedRsiPeriod > 0 && activeMarketPlotData.size() > static_cast<size_t>(validatedRsiPeriod)) { // RSI needs period+1 points
        currentRSIValues = TA::CalculateRSI(activeMarketPlotData, validatedRsiPeriod);
        if (!currentRSIValues.empty()) {
            currentRSITimestamps.reserve(currentRSIValues.size());
            for (size_t i = 0; i < currentRSIValues.size(); ++i) {
                // RSI value at index i corresponds to original data point at index period + i
                currentRSITimestamps.push_back(activeMarketPlotData[validatedRsiPeriod + i].Timestamp);
            }
        }
    }
}

// --- Helper Function: Set Active Market ---
void setActiveMarket(const std::string& name) {
    using namespace AppState;
    activeMarketName = name;

    if (marketGenerators.count(activeMarketName)) {
        activeMarketFullData = marketGenerators[activeMarketName].getData(); // Get all data
        
        // Update plot data (last MAX_PLOT_POINTS)
        activeMarketPlotData.clear();
        int startIdx = std::max(0, static_cast<int>(activeMarketFullData.size()) - MAX_PLOT_POINTS);
        for (size_t i = startIdx; i < activeMarketFullData.size(); ++i) {
            activeMarketPlotData.push_back(activeMarketFullData[i]);
        }
    } else {
        activeMarketFullData.clear();
        activeMarketPlotData.clear();
    }
    updateTAIndicators();
}

// --- Mock main function structure (without actual window/ImGui backend setup) ---
// This function would be your main application entry point.
// For a real application, you need:
// 1. An ImGui backend (e.g., imgui_impl_glfw.cpp + imgui_impl_opengl3.cpp)
// 2. A windowing library (GLFW, SDL, etc.)
// 3. The main loop provided by your backend.
// The content below should be placed INSIDE your application's main loop.
void ApplicationMainLoopIteration() {
    using namespace AppState;

    // --- Real-time Update Logic ---
    float currentTime = ImGui::GetTime();
    if (currentTime - lastUpdateTime >= updateInterval) {
        lastUpdateTime = currentTime;
        if (marketGenerators.count(activeMarketName)) {
            marketGenerators[activeMarketName].generateNewDataPoint(DATA_GENERATION_INTERVAL_SECONDS);
            
            // Efficiently update activeMarketFullData (which is a copy)
            // This is simplified; ideally, MarketDataGenerator would allow getting only new points.
            // For now, we just re-fetch and re-filter for plotting.
            activeMarketFullData = marketGenerators[activeMarketName].getData();
            activeMarketPlotData.clear();
            int startIdx = std::max(0, static_cast<int>(activeMarketFullData.size()) - MAX_PLOT_POINTS);
            for (size_t i = startIdx; i < activeMarketFullData.size(); ++i) {
                activeMarketPlotData.push_back(activeMarketFullData[i]);
            }
            updateTAIndicators();
        }
    }

    // --- ImGui Window Layout ---
    ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    // --- Left Panel (Market Selector & Mini Plot & TA Controls) ---
    ImGui::SetNextWindowSize(ImVec2(250, 0), ImGuiCond_FirstUseEver); // Initial width
    ImGui::Begin("Controls##LeftPanel"); // Added ##LeftPanel for unique ID if "Controls" is used elsewhere

    if (ShowMarketSelector(marketNames, currentMarketIndex, ImGui::GetContentRegionAvail().x)) {
        if (marketNames[currentMarketIndex] != activeMarketName) {
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

    bool smaSettingsChanged = false;
    ImGui::Checkbox("Show SMA", &showSMA); ImGui::SameLine(); 
    ImGui::SetNextItemWidth(100);
    if (ImGui::InputInt("SMA Period", &smaPeriod, 1, 5)) {
        smaPeriod = std::max(1, smaPeriod); // Ensure period is at least 1
        smaSettingsChanged = true;
    }

    bool rsiSettingsChanged = false;
    ImGui::Checkbox("Show RSI", &showRSI); ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    if (ImGui::InputInt("RSI Period", &rsiPeriod, 1, 5)) {
        rsiPeriod = std::max(1, rsiPeriod); // Ensure period is at least 1
        rsiSettingsChanged = true;
    }
    
    if (smaSettingsChanged || rsiSettingsChanged) {
        updateTAIndicators();
    }

    ImGui::End(); // End LeftPanel

    // --- Right Panel (Main Charts) ---
    // ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver); // Example of docking
    ImGui::Begin("Charts##RightPanel");

    if (!activeMarketName.empty()) {
        ShowOHLCPlot(activeMarketName, activeMarketPlotData, showSMA, currentSMAValues, currentSMATimestamps);
        ShowVolumePlot(activeMarketName, activeMarketPlotData); // Default height
        if (showRSI) {
            ShowRSIPlot("ActiveRSI", currentRSIValues, currentRSITimestamps, rsiPeriod); // Default height
        }
    } else {
        ImGui::Text("Select a market to view charts.");
    }

    ImGui::End(); // End RightPanel
}


// Example main() function (conceptual)
int main(int, char**) {
    // --- Initialize Windowing Library (e.g., GLFW) ---
    // --- Initialize ImGui Context ---
    // --- Initialize ImPlot Context ---
    // --- Initialize ImGui Backend (e.g., GLFW + OpenGL3) ---
    // --- Load Fonts, Styles ---

    // Application Initialization
    using namespace AppState;
    marketGenerators.emplace("BTC/USD", MarketDataGenerator::MarketDataGenerator("BTC/USD", 50000.0, 0.005, 100.0));
    marketGenerators.emplace("ETH/USD", MarketDataGenerator::MarketDataGenerator("ETH/USD", 4000.0, 0.008, 200.0));
    marketGenerators.emplace("AAPL", MarketDataGenerator::MarketDataGenerator("AAPL", 170.0, 0.01, 500.0));

    for (auto const& [key, val] : marketGenerators) {
        marketNames.push_back(key);
        // Generate more initial history for MAX_PLOT_POINTS and TA calculations
        marketGenerators[key].generateInitialHistory(MAX_PLOT_POINTS + std::max(smaPeriod, rsiPeriod) + 100, DATA_GENERATION_INTERVAL_SECONDS); 
    }

    if (!marketNames.empty()) {
        currentMarketIndex = 0;
        setActiveMarket(marketNames[currentMarketIndex]);
    }
    lastUpdateTime = ImGui::GetTime(); // Assuming ImGui::GetTime() is available after init


    // --- Main application loop (provided by backend) ---
    // while (!glfwWindowShouldClose(window)) {
    //     glfwPollEvents();
    //     imgui_impl_opengl3_new_frame();
    //     imgui_impl_glfw_new_frame();
    //     ImGui::NewFrame();
    //     ImPlot::NewFrame(); // If using ImPlot new frame semantics
    //
           ApplicationMainLoopIteration(); // Call our application logic
    //
    //     ImGui::Render();
    //     imgui_impl_opengl3_render_draw_data(ImGui::GetDrawData());
    //     glfwSwapBuffers(window);
    // }

    // --- Cleanup ImPlot, ImGui, Windowing library ---
    return 0;
}
// NOTE: The above main() is conceptual. You need a proper ImGui application structure.
// The key part is `ApplicationMainLoopIteration()` and the setup before it.
// This example does not include the ImGui/ImPlot backend setup.
// You would typically call ImGui::CreateContext(), ImPlot::CreateContext(),
// your ImGui_ImplXXX_Init functions, and then the main loop.
