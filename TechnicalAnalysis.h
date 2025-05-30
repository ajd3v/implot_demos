#ifndef TECHNICAL_ANALYSIS_H
#define TECHNICAL_ANALYSIS_H

#include <vector>
#include <numeric>      // For std::accumulate
#include <stdexcept>    // For std::invalid_argument (optional, can also just return empty)
#include <algorithm>    // For std::max/min
#include "MarketDataGenerator.h" // Provides MarketDataPoint definition

namespace TA {

// Calculates Simple Moving Average (SMA) from Close prices.
// Returns a vector of SMA values. The size will be points.size() - period + 1.
// SMA values correspond to the *end* of each period window.
// Corrected from MarketDataGenerator::MarketDataPoint to MarketDataPoint
inline std::vector<double> CalculateSMA(const std::vector<MarketDataPoint>& points, int period) {
    std::vector<double> smaValues;
    if (period <= 0 || static_cast<int>(points.size()) < period) {
        return smaValues; // Not enough data or invalid period
    }

    smaValues.reserve(points.size() - period + 1);
    double currentSum = 0.0;

    // Calculate sum for the first period
    for (int i = 0; i < period; ++i) {
        currentSum += points[i].Close;
    }
    smaValues.push_back(currentSum / period);

    // Slide the window
    for (size_t i = period; i < points.size(); ++i) {
        currentSum -= points[i - period].Close; // Subtract the element that's leaving the window
        currentSum += points[i].Close;         // Add the new element entering the window
        smaValues.push_back(currentSum / period);
    }
    return smaValues;
}


// Calculates Relative Strength Index (RSI) from Close prices.
// Returns a vector of RSI values (scaled 0-100). Size will be points.size() - period.
// RSI values correspond to the *end* of each period window for price changes.
// Corrected from MarketDataGenerator::MarketDataPoint to MarketDataPoint
inline std::vector<double> CalculateRSI(const std::vector<MarketDataPoint>& points, int period) {
    std::vector<double> rsiValues;
    if (period <= 0 || static_cast<int>(points.size()) <= period) { // Need at least period+1 points for 'period' changes
        return rsiValues;
    }

    rsiValues.reserve(points.size() - period);
    std::vector<double> gains;
    std::vector<double> losses;
    gains.reserve(points.size() -1);
    losses.reserve(points.size() -1);

    // Calculate initial gains and losses
    for (size_t i = 1; i < points.size(); ++i) {
        double change = points[i].Close - points[i - 1].Close;
        if (change > 0) {
            gains.push_back(change);
            losses.push_back(0.0);
        } else {
            gains.push_back(0.0);
            losses.push_back(-change); // Store losses as positive values
        }
    }

    if (gains.empty()) return rsiValues; // No changes in price data

    double avgGain = 0.0;
    double avgLoss = 0.0;

    // Calculate initial average gain and loss for the first 'period' changes
    for (int i = 0; i < period; ++i) {
        avgGain += gains[i];
        avgLoss += losses[i];
    }
    avgGain /= period;
    avgLoss /= period;

    double rs = (avgLoss == 0) ? 100.0 : avgGain / avgLoss; // Avoid division by zero; if no losses, RSI is 100
    rsiValues.push_back(100.0 - (100.0 / (1.0 + rs)));

    // Calculate subsequent RSI values using smoothed average
    for (size_t i = period; i < gains.size(); ++i) {
        avgGain = ((avgGain * (period - 1)) + gains[i]) / period;
        avgLoss = ((avgLoss * (period - 1)) + losses[i]) / period;

        rs = (avgLoss == 0) ? 100.0 : avgGain / avgLoss;
        rsiValues.push_back(100.0 - (100.0 / (1.0 + rs)));
    }

    return rsiValues;
}

} // namespace TA

#endif // TECHNICAL_ANALYSIS_H
