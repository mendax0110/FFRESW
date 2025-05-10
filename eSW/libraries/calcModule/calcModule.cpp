/**
 * @file calcModule.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "calcModule.h"
#include <Arduino.h>
#include <math.h>

using namespace calcModule;

CalcModuleInternals::CalcModuleInternals()
{

}

CalcModuleInternals::~CalcModuleInternals()
{

}

float CalcModuleInternals::calculateAverage(const float* data, int length)
{
    if (length <= 0 || data == nullptr) return 0.0;
    float sum = 0.0;
    for (int i = 0; i < length; ++i)
    {
        sum += data[i];
    }
    float result = sum / length;
    return roundToPrecision(result, 5);
}

float CalcModuleInternals::findMaximum(const float* data, int length)
{
    if (length <= 0 || data == nullptr) return 0.0;
    float max = data[0];
    for (int i = 1; i < length; ++i)
    {
        if (data[i] > max)
        {
            max = data[i];
        }
    }
    return roundToPrecision(max, 5);
}

float CalcModuleInternals::findMinimum(const float* data, int length)
{
    if (length <= 0 || data == nullptr) return 0.0;
    float min = data[0];
    for (int i = 1; i < length; ++i)
    {
        if (data[i] < min)
        {
            min = data[i];
        }
    }
    return roundToPrecision(min, 5);
}

float CalcModuleInternals::calculateStandardDeviation(const float* data, int length)
{
    if (length <= 0 || data == nullptr) return 0.0;
    float mean = calculateAverage(data, length);
    float variance = 0.0;
    for (int i = 0; i < length; ++i)
    {
        variance += (data[i] - mean) * (data[i] - mean);
    }
    variance /= length;
    return roundToPrecision(sqrtf(variance), 5);
}

float CalcModuleInternals::findMedian(float* data, int length)
{
    if (length <= 0 || data == nullptr) return 0.0;
    sortArray(data, length);
    float result;
    if (length % 2 == 0)
    {
        result = (data[length / 2 - 1] + data[length / 2]) / 2.0;
        return roundToPrecision(result, 5);
    }
    else
    {
        result = data[length / 2];
        return roundToPrecision(result, 5);
    }
}

void CalcModuleInternals::sortArray(float* data, int length)
{
    for (int i = 0; i < length - 1; ++i)
    {
        for (int j = 0; j < length - i - 1; ++j)
        {
            if (data[j] > data[j + 1])
            {
                // Swap data[j] and data[j + 1]
                float temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }
}

float CalcModuleInternals::celsiusToFahrenheit(float celsius)
{
	float result = (celsius * 9.0 / 5.0) + 32.0;
    return roundToPrecision(result, 5);
}

float CalcModuleInternals::fahrenheitToCelsius(float fahrenheit)
{
	float result = (fahrenheit - 32.0) * 5.0 / 9.0;
    return roundToPrecision(result, 5);
}

float CalcModuleInternals::celsiusToKelvin(float celsius)
{
	float result = celsius + 273.15;
    return roundToPrecision(result, 5);
}

float CalcModuleInternals::kelvinToCelsius(float kelvin)
{
	float result = kelvin - 273.15;
    return roundToPrecision(result, 5);
}

float CalcModuleInternals::pascalToAtm(float pascal)
{
    float result =  pascal / 101325.0; // 1 atm = 101325 Pa
    return roundToPrecision(result, 5);
}

float CalcModuleInternals::atmToPascal(float atm)
{
    float result = atm * 101325.0;
    return roundToPrecision(result, 5);
}

float CalcModuleInternals::pascalToPsi(float pascal)
{
	float result = pascal * 0.000145038; // 1 Pa = 0.000145038 psi
    return roundToPrecision(result, 5);
}

float CalcModuleInternals::psiToPascal(float psi)
{
	float result = psi * 6894.76; // 1 psi = 6894.76 Pa
    return roundToPrecision(result, 5);
}

float CalcModuleInternals::calculatePower(float voltage, float current)
{
	float result =voltage * current; // Power (P = V * I)
    return roundToPrecision(result, 5);
}

float CalcModuleInternals::calculateCurrent(float voltage, float resistance)
{
    if (resistance == 0) return 0.0; // Avoid division by zero
    float result = voltage / resistance; // Current (I = V / R)
    return roundToPrecision(result, 5);
}

float CalcModuleInternals::calculateResistance(float voltage, float current)
{
    if (current == 0) return 0.0; // Avoid division by zero
    float result = voltage / current; // Resistance (R = V / I)
    return roundToPrecision(result, 5);
}

float CalcModuleInternals::extractFloat(String response, int id)
{
	String extracted;

    if (id == 0)
    {
        int startIndex = response.indexOf("p:");
        if (startIndex == -1)
        {
            Serial.println(F("p: not found"));
            return 0.0f;
        }

        startIndex += 2;


        if (response.startsWith("000b", startIndex))
        {
            startIndex += 4;
        }

        int hexLength = 8;
        startIndex += hexLength;

        int numStartIndex = startIndex;

        while (numStartIndex < response.length() &&
               !isdigit(response.charAt(numStartIndex)) &&
               response.charAt(numStartIndex) != '-')
        {
            numStartIndex++;
        }

        if (numStartIndex < response.length() && response.charAt(numStartIndex) == '-')
        {
            numStartIndex++;
        }

        int numEndIndex = numStartIndex;
        while (numEndIndex < response.length() &&
               (isdigit(response.charAt(numEndIndex)) ||
                response.charAt(numEndIndex) == '.' ||
                response.charAt(numEndIndex) == '-'))
        {
            numEndIndex++;
        }

        extracted = response.substring(numStartIndex, numEndIndex);
        extracted.trim();

        if (extracted.startsWith("00"))
        {
            extracted = extracted.substring(2);
        }

        if (extracted == ".0" || extracted == "")
        {
            extracted = "0";
        }
    }
	else if (id == 1)
	{
		// TODO: Implement handling for compound 1
	}
	else if (id == 2)
	{
		// TODO: Implement handling for compound 2
	}
	else if (id == 3)
	{
		// TODO: Implement handling for compound 3
	}

    return extracted.toFloat();
}

float CalcModuleInternals::extractFloatFromResponse(const String& response, Type type)
{
	const char* prefix = nullptr;

	switch (type)
	{
		case Type::Pressure:
			prefix = "p:000b07010000";
			break;
		case Type::Position:
			prefix = "p:000b10010000";
			break;
		default:
			return 0.0f;
	}

	const int prefixLen = strlen(prefix);
	int startIdx = 0;

	while (startIdx < response.length())
	{
		int endIdx = response.indexOf('\n', startIdx);
		if (endIdx == -1) endIdx = response.length();

		String line = response.substring(startIdx, endIdx);
		line.trim();

		if (line.startsWith(prefix))
		{
			String valueStr = line.substring(prefixLen);
			valueStr.trim();
			float value = valueStr.toFloat();
			if (value > 0)
			{
				return value;
			}
		}

		startIdx = endIdx + 1;
	}

	return -1.0f;
}

float CalcModuleInternals::roundToPrecision(float value, int precision)
{
	float factor = pow(10, precision);
	return round(value * factor) / factor;
}

