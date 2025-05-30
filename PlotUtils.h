#ifndef PLOT_UTILS_H
#define PLOT_UTILS_H

#include <string>
#include <chrono>
#include <iomanip> // For std::put_time
#include <sstream> // For std::ostringstream
#include <cmath>   // For std::abs, std::fabs
#include <limits>  // For std::numeric_limits

// Helper function to format a Unix timestamp into "YYYY-MM-DD HH:MM:SS"
inline std::string FormatTimestampForTooltip(double unixTimestamp) {
    if (unixTimestamp <= 0) return "N/A";
    std::time_t time_t_stamp = static_cast<std::time_t>(unixTimestamp);
    std::tm tm_struct;

#ifdef _WIN32
    localtime_s(&tm_struct, &time_t_stamp); // Windows specific
#else
    localtime_r(&time_t_stamp, &tm_struct); // POSIX specific
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm_struct, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// Helper function to find the index of the closest data point to the mouse's X-position
// Assumes timestamps are sorted.
inline int FindClosestPointIndex(const std::vector<double>& timestamps, double mouseX) {
    if (timestamps.empty()) {
        return -1;
    }

    int closestIndex = -1;
    double minDiff = std::numeric_limits<double>::max();

    // Simple linear scan, can be optimized with binary search for very large datasets
    for (size_t i = 0; i < timestamps.size(); ++i) {
        double diff = std::abs(timestamps[i] - mouseX);
        if (diff < minDiff) {
            minDiff = diff;
            closestIndex = static_cast<int>(i);
        }
    }
    return closestIndex;
}

// Overload for MarketDataPoint vector, searching based on MarketDataPoint.Timestamp
inline int FindClosestPointIndex(const std::vector<MarketDataGenerator::MarketDataPoint>& dataPoints, double mouseX) {
    if (dataPoints.empty()) {
        return -1;
    }

    int closestIndex = -1;
    double minDiff = std::numeric_limits<double>::max();

    for (size_t i = 0; i < dataPoints.size(); ++i) {
        double diff = std::abs(dataPoints[i].Timestamp - mouseX);
        if (diff < minDiff) {
            minDiff = diff;
            closestIndex = static_cast<int>(i);
        }
    }
    return closestIndex;
}


#endif // PLOT_UTILS_H
