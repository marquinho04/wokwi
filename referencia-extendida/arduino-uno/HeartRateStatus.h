/*
  HeartRateStatus.h
  Smart LifeWatching - Embedded Application (Modest IoT based)

  Heart rate status and its classification rules:
  * less than 60 BPM  -> Low    (blue LED)
  * 60 to 100 BPM     -> Normal (green LED)
  * more than 100 BPM -> High   (red LED)

  Created 19 November 2025
  By <Developer Full Name> - Smart LifeMonitor, inc.
*/

#ifndef HEART_RATE_STATUS_H
#define HEART_RATE_STATUS_H

/**
 * @brief Possible states of a heart rate measurement.
 */
enum class HeartRateStatus {
    NoSignal,
    Low,
    Normal,
    High
};

/**
 * @brief Classifies heart rate values and provides their display text.
 */
class HeartRateClassifier {
public:
    static const int NORMAL_MINIMUM_BEATS_PER_MINUTE = 60;  ///< Inclusive lower limit of Normal.
    static const int NORMAL_MAXIMUM_BEATS_PER_MINUTE = 100; ///< Inclusive upper limit of Normal.

    /**
     * @brief Classifies a valid heart rate value.
     * @param beatsPerMinute Heart rate in BPM.
     * @return Low, Normal or High.
     */
    static HeartRateStatus classify(int beatsPerMinute) {
        if (beatsPerMinute < NORMAL_MINIMUM_BEATS_PER_MINUTE) {
            return HeartRateStatus::Low;
        }
        if (beatsPerMinute > NORMAL_MAXIMUM_BEATS_PER_MINUTE) {
            return HeartRateStatus::High;
        }
        return HeartRateStatus::Normal;
    }

    /**
     * @brief Returns the text shown on the display / console for a status.
     */
    static const char* toText(HeartRateStatus status) {
        switch (status) {
            case HeartRateStatus::Low:    return "LOW";
            case HeartRateStatus::Normal: return "NORMAL";
            case HeartRateStatus::High:   return "HIGH";
            default:                      return "NO SIGNAL";
        }
    }
};

#endif // HEART_RATE_STATUS_H
