#ifndef MARKET_DATA_GENERATOR_H
#define MARKET_DATA_GENERATOR_H

#include <vector>
#include <string>
#include <random>
#include <cmath>
#include <algorithm>
#include <chrono> // For timestamps

// Data Structure
struct MarketDataPoint {
    double Timestamp;
    double Open;
    double High;
    double Low;
    double Close;
    double Volume;
};

class MarketDataGenerator {
public:
    // Constructor
    MarketDataGenerator(std::string marketName, double initialPrice, double typicalPriceVolatility, double typicalVolume);

    // Methods
    void generateInitialHistory(int numPoints, double timeStepSeconds);
    MarketDataPoint generateNewDataPoint(double timeStepSeconds);
    const std::vector<MarketDataPoint>& getData() const;

private:
    std::string marketName_;
    double initialPrice_;
    double typicalPriceVolatility_;
    double typicalVolume_;
    std::vector<MarketDataPoint> dataPoints_;
    std::mt19937 randomEngine_; // For random number generation
    double lastTimestamp_; // Keep track of the last timestamp used

    // Helper for generating a single point, used by both public methods
    MarketDataPoint generateNextPoint(double previousClose, double currentTimestamp, double timeStepSeconds);
    double getCurrentTimestampEpoch(); // Helper to get current time as double (seconds since epoch)
};

// Constructor Implementation
MarketDataGenerator::MarketDataGenerator(std::string marketName, double initialPrice, double typicalPriceVolatility, double typicalVolume)
    : marketName_(std::move(marketName)),
      initialPrice_(initialPrice),
      typicalPriceVolatility_(typicalPriceVolatility),
      typicalVolume_(typicalVolume),
      lastTimestamp_(0.0) {
    // Seed the random engine
    std::random_device rd;
    randomEngine_.seed(rd());
}

// Helper to get current time as double (seconds since epoch)
inline double MarketDataGenerator::getCurrentTimestampEpoch() {
    return std::chrono::duration_cast<std::chrono::duration<double>>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

// Private helper method to generate the next data point
inline MarketDataPoint MarketDataGenerator::generateNextPoint(double previousClose, double currentTimestamp, double timeStepSeconds) {
    MarketDataPoint point;
    point.Timestamp = currentTimestamp;
    point.Open = previousClose;

    // 1. Determine Close price
    // Fluctuation is proportional to Open price and volatility
    std::uniform_real_distribution<double> priceFluctuationDist(-typicalPriceVolatility_, typicalPriceVolatility_);
    double fluctuation = priceFluctuationDist(randomEngine_);
    point.Close = point.Open * (1.0 + fluctuation);

    // Ensure Close is positive
    if (point.Close <= 0.0001) {
        point.Close = 0.0001;
    }

    // 2. Determine High and Low
    double candleBodyHigh = std::max(point.Open, point.Close);
    double candleBodyLow = std::min(point.Open, point.Close);

    // Wicks are random positive extensions, also proportional to Open price and volatility
    // The factor (e.g., 0.5) for wick randomness can be adjusted.
    std::uniform_real_distribution<double> upperWickDist(0.0, typicalPriceVolatility_ * 0.5 * point.Open);
    std::uniform_real_distribution<double> lowerWickDist(0.0, typicalPriceVolatility_ * 0.5 * point.Open);

    point.High = candleBodyHigh + upperWickDist(randomEngine_);
    point.Low = candleBodyLow - lowerWickDist(randomEngine_);

    // Ensure Low is positive
    if (point.Low <= 0.0001) {
        point.Low = 0.0001;
    }

    // After wicks are added to candleBodyHigh and candleBodyLow to get point.High and point.Low:
    // point.High is already guaranteed to be >= candleBodyHigh (i.e. >= Open and Close)
    // point.Low is already guaranteed to be <= candleBodyLow (i.e. <= Open and Close)

    // Ensure Low is positive (already done before, but good to keep if there's any doubt)
    if (point.Low <= 0.0001) {
        point.Low = 0.0001;
    }

    // Ensure High is greater than or equal to Low.
    // This can only fail if Open price is extremely small, making wicks disproportional
    // or if Low was clamped at 0.0001 and High ended up less.
    if (point.High < point.Low) {
        point.High = point.Low; // Or point.High = point.Low + some_minimal_spread if preferred
    }

    // Generate Volume
    // Fluctuate volume by +/- 50% of typicalVolume_ as an example
    std::uniform_real_distribution<double> volumeFluctuationDist(-0.5, 0.5); // Max 50% fluctuation
    point.Volume = typicalVolume_ + (typicalVolume_ * volumeFluctuationDist(randomEngine_));
    if (point.Volume <= 0) {
        point.Volume = std::max(1.0, typicalVolume_ * 0.1); // Ensure volume is positive, at least 1 or 10% of typical
    }

    return point;
}

// Public Methods Implementation

inline void MarketDataGenerator::generateInitialHistory(int numPoints, double timeStepSeconds) {
    dataPoints_.clear();
    dataPoints_.reserve(numPoints);

    // Initialize lastTimestamp_ to "numPoints * timeStepSeconds" ago from current time for a more realistic start
    // Or simply start from a conventional 0 or a fixed past date if preferred.
    // For this example, let's start from a fixed point for predictability if needed, or current time.
    // lastTimestamp_ = getCurrentTimestampEpoch() - (static_cast<double>(numPoints) * timeStepSeconds);
    
    // Let's make the first timestamp predictable for potential testing/reproducibility if not using current time.
    // If we want it relative to 'now' when called:
    // lastTimestamp_ = getCurrentTimestampEpoch() - (static_cast<double>(numPoints + 1) * timeStepSeconds);
    // For simplicity in this generator, let's assume the first call to generateNewDataPoint or generateInitialHistory sets the initial time basis.
    // If dataPoints_ is empty, the first point's timestamp is based on call time or a fixed value.

    double currentPrice = initialPrice_;
    // Calculate the timestamp for the very first historical data point.
    // This makes the history lead up to "just before now".
    double iterationTimestamp = getCurrentTimestampEpoch() - (static_cast<double>(numPoints) * timeStepSeconds);

    for (int i = 0; i < numPoints; ++i) {
        // If it's the first point and initialPrice_ is used, ensure Open=High=Low=Close for the first "previousClose"
        // However, generateNextPoint naturally handles this by setting Open = previousClose.
        // For the very first point, we can imagine a "flat" candle just before it, whose close was initialPrice.
        MarketDataPoint newPoint = generateNextPoint(currentPrice, iterationTimestamp, timeStepSeconds);
        dataPoints_.push_back(newPoint);
        currentPrice = newPoint.Close;
        iterationTimestamp += timeStepSeconds;
    }

    // After the loop, iterationTimestamp is one step *past* the last generated point's timestamp.
    // So, the actual last timestamp is iterationTimestamp - timeStepSeconds.
    if (!dataPoints_.empty()) {
        lastTimestamp_ = dataPoints_.back().Timestamp;
    } else {
        // If numPoints was 0, lastTimestamp_ should be based on initial setup or remain 0.0
        // For consistency, make it the "current time" if history is empty.
        lastTimestamp_ = getCurrentTimestampEpoch() - timeStepSeconds; 
    }
}

inline MarketDataPoint MarketDataGenerator::generateNewDataPoint(double timeStepSeconds) {
    double previousClose;
    double nextPointTimestamp;

    if (dataPoints_.empty()) {
        previousClose = initialPrice_;
        // If lastTimestamp_ is 0.0 (no history, no prior new points), new point is "now".
        // Otherwise, if lastTimestamp_ was set by generateInitialHistory(0 points), it's "now - step".
        if (lastTimestamp_ == 0.0) {
            nextPointTimestamp = getCurrentTimestampEpoch();
        } else { // lastTimestamp_ was set by generateInitialHistory(0) or a previous generateNewDataPoint
            nextPointTimestamp = lastTimestamp_ + timeStepSeconds;
        }
    } else {
        previousClose = dataPoints_.back().Close;
        // lastTimestamp_ is the timestamp of the last point in dataPoints_
        nextPointTimestamp = lastTimestamp_ + timeStepSeconds;
    }
    
    MarketDataPoint newPoint = generateNextPoint(previousClose, nextPointTimestamp, timeStepSeconds);
    
    dataPoints_.push_back(newPoint);
    lastTimestamp_ = newPoint.Timestamp; // Update master timestamp to the newest point's timestamp
    
    return newPoint;
}

inline const std::vector<MarketDataPoint>& MarketDataGenerator::getData() const {
    return dataPoints_;
}

#endif // MARKET_DATA_GENERATOR_H
