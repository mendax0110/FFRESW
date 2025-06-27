#include <neutronDetector.h>
#include <stdint.h>
#include <serialMenu.h>

NeutronDetector::NeutronDetector(uint8_t analogPin, uint16_t threshold)
	: _pin(analogPin)
	, _threshold(threshold)
	, _writeIndex(0)
	, _storedCount(0)
	, _lastCaptureTime(0)
	, _neutronDetectorInitialized(true)
{

}

void NeutronDetector::initialize()
{
	pinMode(_pin, INPUT);

	_neutronDetectorInitialized = true;
	SerialMenu::printToSerial(F("[INFO] NeutronDetector module initialized."));
}

bool NeutronDetector::isInitialized() const
{
	return _neutronDetectorInitialized;
}

bool NeutronDetector::checkInputConnected()
{
    int stableReadings = 0;
    for(int i=0; i<10; i++)
    {
        int val = analogRead(_pin);
        if(val > 10 && val < 1010)
        {
            stableReadings++;
        }
        delayMicroseconds(100);
    }
    return (stableReadings >= 8);
}

void NeutronDetector::update()
{
	uint32_t now = micros();

    // Periodically check connection
    if(now - _lastConnectionCheck > CONNECTION_CHECK_INTERVAL)
    {
        _inputConnected = checkInputConnected();
        _lastConnectionCheck = now;

        if(!_inputConnected)
        {
            reset(); // Clear any stored pulses
            return;
        }
    }

    if(!_inputConnected) return;

    updateBaseline();
    if (now - _lastCaptureTime >= _minInterval)
    {
        uint16_t val = overSample(true);
        if (val - _baseline >= _threshold)
        {
            capturePulse();
            _lastCaptureTime = now;
        }
    }
}

void NeutronDetector::capturePulse()
{
    Pulse &p = _pulses[_writeIndex];
    p.timestamp = micros();
    uint8_t peak = 0;
    uint32_t sampleStart = micros();

    for (uint8_t i = 0; i < SAMPLES_PER_PULSE; ++i)
    {
        while (micros() - sampleStart < i * SAMPLE_INTERVAL_US) {}

        uint16_t raw = overSample(true);
        if (raw >= MAX_OVERSAMPLED_VALUE) return;

        p.samples[i] = raw >> 2;  // Convert 10-bit to 8-bit
        if (p.samples[i] > peak) peak = p.samples[i];
    }

    p.peakValue = peak;
    _writeIndex = (_writeIndex + 1) % MAX_PULSES;
    _storedCount = min(_storedCount + 1, MAX_PULSES);
}

uint8_t NeutronDetector::getPulseCount() const
{
	return _storedCount;
}

const NeutronDetector::Pulse& NeutronDetector::getPulse(uint8_t index) const
{
	if (index >= _storedCount)
	{
		static Pulse defaultPulse = {0};
		return defaultPulse;
	}

	uint8_t actualIndex = (_writeIndex + MAX_PULSES - _storedCount + index) % MAX_PULSES;

	return _pulses[actualIndex];
}

NeutronDetector::PulseAnalysis NeutronDetector::getPulseAnalysis(uint8_t index) const
{
	return analyzePulse(getPulse(index));
}

bool NeutronDetector::isInputConnected() const
{
	return _inputConnected;
}

void NeutronDetector::reset()
{
	_writeIndex = 0;
	_storedCount = 0;
}

void NeutronDetector::updateBaseline()
{
	uint16_t newReading = overSample(true);
	int16_t dev = newReading - _baseline;
	_baseline = 0.9 * _baseline + 0.1 * newReading; // LP-Filter

	if (abs(dev) > BASELINE_DEVIATION_THRESHOLD)
	{
		updateThreshold(dev);
	}
}

uint16_t NeutronDetector::overSample(bool active)
{
    if (!active) return analogRead(_pin);

    uint32_t sum = 0;
    uint32_t start = micros();

    for (uint8_t i = 0; i < 16; i++)
    {
        sum += analogRead(_pin);
        while (micros() - start < i * OVERSAMPLE_INTERVAL_US) {}
    }

    return sum >> 2;
}

void NeutronDetector::updateThreshold(uint16_t currentDev)
{
	static uint16_t noiseRMS = 40;
	noiseRMS = 0.9 * noiseRMS + 0.1 * abs(currentDev);
	noiseRMS = max(noiseRMS, 2);
	_threshold = _baseline + 4 * noiseRMS;
}

float NeutronDetector::computeDecayTime(const Pulse& p) const
{
	uint16_t peak = 0;
	uint8_t peakIndex = 0;
	bool foundPeak = false;

	for (uint8_t i = 0; i < SAMPLES_PER_PULSE; ++i)
	{
		if (p.samples[i] > MAX_SAMPLE_VALUE) return -1.0f;
		if (p.samples[i] > peak)
		{
			peak = p.samples[i];
			peakIndex = i;
			foundPeak = true;
		}
	}

	if (!foundPeak || peak < MIN_PULSE_AMPLITUDE) return -1.0f;

    const uint8_t threshold = peak * 0.1f;
    for (uint8_t i = peakIndex; i < SAMPLES_PER_PULSE; ++i)
    {
        if (p.samples[i] < threshold)
        {
            return (i - peakIndex) * SAMPLE_INTERVAL_US;
        }
    }

	return -1.0;
}

float NeutronDetector::computePulseArea(const Pulse& p) const
{
    float area = 0;
    for (uint8_t i = 0; i < SAMPLES_PER_PULSE - 1; ++i)
    {
        area += (p.samples[i] + p.samples[i + 1]) / 2.0 * SAMPLE_INTERVAL_US;
    }
    return area;
}

float NeutronDetector::computeRiseTime(const Pulse& p) const
{
	float peak = 0;
	for (uint8_t i = 0; i < SAMPLES_PER_PULSE; ++i)
	{
		if (p.samples[i] > MAX_SAMPLE_VALUE) return -1.0f;
		if (p.samples[i] > peak)
		{
			peak = p.samples[i];
		}
	}

	const float threshold10 = 0.1f * peak;
	const float threshold90 = 0.9f * peak;
	uint8_t t10 = 0;
	uint8_t t90 = 0;

	for (uint8_t i = 0; i < SAMPLES_PER_PULSE; ++i)
	{
		if (p.samples[i] >= threshold10)
		{
			t10 = i;
			break;
		}
	}

	for (uint8_t i = t10; i < SAMPLES_PER_PULSE; ++i)
	{
		if (p.samples[i] >= threshold90)
		{
			t90 = i;
			break;
		}
	}

	return (t90 - t10) * SAMPLE_INTERVAL_US;
}

NeutronDetector::PulseAnalysis NeutronDetector::analyzePulse(const Pulse& p) const
{
    PulseAnalysis result;
    result.pulseArea = computePulseArea(p);
    result.decayTime = computeDecayTime(p);
    result.riseTime = computeRiseTime(p);

    result.isNeutron = (result.decayTime > NEUTRON_DECAY_TIME_THRESHOLD) &&
    					(result.riseTime > NEUTRON_RISE_TIME_THRESHOLD) &&
						(result.pulseArea > NEUTRON_AREA_THRESHOLD);

    return result;
}
