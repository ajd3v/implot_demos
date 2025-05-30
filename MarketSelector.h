#ifndef MARKET_SELECTOR_H
#define MARKET_SELECTOR_H

#include <vector>
#include <string>
#include "imgui.h" // Assuming imgui.h is in the include path

// selectedMarketIndex (in/out): Tracks the index of the currently selected market.
// marketNames (input): A list of market names to display.
// PUSH_WINDOW_WIDTH (input): The width of the child window for the market selector.
// Returns true if a new market is selected, false otherwise.
inline bool ShowMarketSelector(const std::vector<std::string>& marketNames, int& currentMarketIndex, float PUSH_WINDOW_WIDTH = 150.0f) {
    bool selectionChanged = false;

    // Begin a child window for the market selector.
    // Using ImGuiWindowFlags_HorizontalScrollbar can be added if market names are very long,
    // but vertical scrollbar is the primary concern for a list.
    ImGui::BeginChild("MarketSelectorChild", ImVec2(PUSH_WINDOW_WIDTH, 0), true, ImGuiWindowFlags_None);

    for (int i = 0; i < marketNames.size(); ++i) {
        // Ensure marketNames[i] is not null if it comes from C-style strings
        if (marketNames[i].empty()) continue;

        if (ImGui::Selectable(marketNames[i].c_str(), currentMarketIndex == i)) {
            if (currentMarketIndex != i) { // Check if the selection actually changed
                currentMarketIndex = i;
                selectionChanged = true;
            }
        }
    }

    ImGui::EndChild();

    return selectionChanged;
}

#endif // MARKET_SELECTOR_H
