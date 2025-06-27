#ifndef NEUTRON_DETECTOR_H
#define NEUTRON_DETECTOR_H

#include <Arduino.h>


/// @brief This class takes care of handling the input from the NeutronDetector. \class NeutronDetector
class NeutronDetector
{
public:

    static constexpr uint8_t SAMPLES_PER_PULSE = 20;
    static constexpr uint8_t MAX_PULSES = 10;
    static constexpr uint16_t SAMPLE_INTERVAL_US = 10;
    static constexpr uint16_t OVERSAMPLE_INTERVAL_US = 2;

	/**
	 * @brief This struct stores relevant Pulse info from the detector. \struct Pulse
	 */
	struct Pulse
	{
		uint32_t timestamp;
		uint8_t samples[SAMPLES_PER_PULSE];
		uint8_t peakValue;
	};
	
	/**
	 *@brief This typedef struct stores all values for the PulseAnalysis \struct PulseAnalysis
	 */
	typedef struct
	{
		float decayTime;
		float riseTime;
		float pulseArea;
		bool isNeutron;
	} PulseAnalysis;

	/**
	 * @brief Constructor for the NeutronDetector
	 * @param analogPin The analogPin we read from.
	 * @param threshold The threshold we need to set.
	 */
	NeutronDetector(uint8_t analogPin = A0 , uint16_t threshold = 100); // TODO TELL FRANIC TO UPDATE -> https://docs.google.com/spreadsheets/d/1-Il1e9477HAMj91pHT6NhEZsUtf2OpcX/edit?gid=855028862#gid=855028862


	/**
	 * @brief Function to start the neutron detection.
	 */
	void initialize();
	
	/**
	 * @brief Function check if detector is init.
	 */
	bool isInitialized() const;

	/**
	 * @brief Function to update the detection info.
	 */
	void update();
	
	/**
	 * @brief Resets the circular buffer vars e.g _writeIndex and _storedCount.
	 */
	void reset();

	/**
	 * @brief Getter for the Pulse count.
	 * @return A integer with the count.
	 */
	uint8_t getPulseCount() const;
	
	/**
	 * @brief Gets stored pulses in chronological order.
	 * @return A Pulse Object ref with the relevant information.
	 */
	const Pulse& getPulse(uint8_t index) const;
	
	
	/**
	 * @brief Gets stored analyzed pulse.
	 * @return A analyzed pulse Object ref with the relevant information.
	 */
	PulseAnalysis getPulseAnalysis(uint8_t index) const;

	/**
	 * @brief Checks if the Input is connected otherwise, we cannot read -> only noise.
	 * @return A true if the Input is connected otherwise false.
	 */
	bool isInputConnected() const;

private:
	uint8_t _pin;
	uint16_t _threshold;
	Pulse _pulses[MAX_PULSES];
	uint8_t _writeIndex;
	uint8_t _storedCount;
	
	unsigned long _lastCaptureTime = 0;
	const unsigned long _minInterval = 5000;
	
	uint16_t _baseline = 512;
	
    static constexpr uint16_t MAX_OVERSAMPLED_VALUE = 1023;
    static constexpr uint8_t MAX_SAMPLE_VALUE = 225;
    static constexpr uint8_t MIN_PULSE_AMPLITUDE = 10;
    static constexpr float NEUTRON_DECAY_TIME_THRESHOLD = 25.0f;
    static constexpr float NEUTRON_RISE_TIME_THRESHOLD = 12.0f;
    static constexpr float NEUTRON_AREA_THRESHOLD = 500.0f;
    static constexpr uint8_t BASELINE_DEVIATION_THRESHOLD = 5;

    bool _neutronDetectorInitialized;
    bool _inputConnected = false;
    uint32_t _lastConnectionCheck = 0;
    const uint32_t CONNECTION_CHECK_INTERVAL = 1000000;
	
	/**
	 * @brief Captures the Pulses and stores them in the circular buffer.
	 */
	void capturePulse();
	
	/**
	 * @brief Method to update the baseline for the measurement.
	 */
	void updateBaseline();
	
	/**
	 * @brief Method to activate overSampling.
	 *
	 */
	uint16_t overSample(bool active);
	
	/**
	 * @brief Algo to calculate the decay time to differntiate between (Fast neutron, Gamma rays).
	 * @brief p The object refernce to the pulse object.
	 * @return A float with the decayTime.
	 */
	float computeDecayTime(const Pulse& p) const;
	
	/**
	 * @brief Algo to calculate the Pulse area.
	 * @param p The object reference to the pulse object.
	 * @return A float with the PulseArea.
	 */
	float computePulseArea(const Pulse& p) const;
	
	/**
	 * @brief Algo to calculate the Rise Time.
	 * @param p The object reference to the pulse object.
	 * @return A float with the PulseArea.
	 */
	float computeRiseTime(const Pulse& p) const;
	
	/**
	 * @brief Helper method to update the Threshold.
	 * @param currentDev The actual current deviation.
	 */
	void updateThreshold(uint16_t currentDev);
	
	/**
	 * @brief Method to analyse DecayTime, PulseArea and RiseTime.
	 * @param p The object reference to the pulse object.
	 * @return A PulseAnalysis Object.
	 */
	PulseAnalysis analyzePulse(const Pulse& p) const;

	/**
	 * @brief Checks for stable readings to detemine if the Input is connected or just noise.
	 * @return True if Input is connected, false otherwise.
	 */
	bool checkInputConnected();

};

#endif // NEUTRON_DETECTOR_H
