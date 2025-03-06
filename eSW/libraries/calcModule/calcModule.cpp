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

/// @brief Function to calcuate the Average of the given data
/// @param data -> This is the data array
/// @param length -> This is the length of the data array
/// @return float -> This returns the average of the data
float CalcModuleInternals::calculateAverage(const float* data, int length)
{
    if (length <= 0 || data == nullptr) return 0.0;
    float sum = 0.0;
    for (int i = 0; i < length; ++i)
    {
        sum += data[i];
    }
    return sum / length;
}

/// @brief Function to find the Maximum value in the given data
/// @param data -> This is the data array
/// @param length -> This is the length of the data array
/// @return float -> This returns the maximum value in the data
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
    return max;
}

/// @brief Function to find the Minimum value in the given data
/// @param data -> This is the data array
/// @param length -> This is the length of the data array
/// @return flaot -> This returns the minimum value in the data
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
    return min;
}

/// @brief Function to calculate the Standard Deviation of the given data
/// @param data -> This is the data array
/// @param length -> This is the length of the data array
/// @return float -> This returns the standard deviation of the data
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
    return sqrtf(variance);
}

/// @brief Function to find the Median of the given data
/// @param data -> This is the data array
/// @param length -> This is the length of the data array
/// @return float -> This returns the median of the data
float CalcModuleInternals::findMedian(float* data, int length)
{
    if (length <= 0 || data == nullptr) return 0.0;
    sortArray(data, length);
    if (length % 2 == 0)
    {
        return (data[length / 2 - 1] + data[length / 2]) / 2.0;
    }
    else
    {
        return data[length / 2];
    }
}

/// @brief Function to sort the given array in ascending order
/// @param data -> This is the data array
/// @param length -> This is the length of the data array
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

/// @brief Function to convert Celsius to Fahrenheit
/// @param celsius -> This is the temperature in Celsius
/// @return float -> This returns the temperature in Fahrenheit
float CalcModuleInternals::celsiusToFahrenheit(float celsius)
{
    return (celsius * 9.0 / 5.0) + 32.0;
}

/// @brief Function to convert Fahrenheit to Celsius
/// @param fahrenheit -> This is the temperature in Fahrenheit
/// @return float -> This returns the temperature in Celsius
float CalcModuleInternals::fahrenheitToCelsius(float fahrenheit)
{
    return (fahrenheit - 32.0) * 5.0 / 9.0;
}

/// @brief Function to convert Celsius to Kelvin
/// @param celsius -> This is the temperature in Celsius
/// @return float -> This returns the temperature in Kelvin
float CalcModuleInternals::celsiusToKelvin(float celsius)
{
    return celsius + 273.15;
}

/// @brief Function to convert Kelvin to Celsius
/// @param kelvin -> This is the temperature in Kelvin
/// @return float -> This returns the temperature in Celsius
float CalcModuleInternals::kelvinToCelsius(float kelvin)
{
    return kelvin - 273.15;
}

/// @brief Function to convert Pascal to Atmosphere
/// @param pascal -> This is the pressure in Pascal
/// @return float -> This returns the pressure in Atmosphere
float CalcModuleInternals::pascalToAtm(float pascal)
{
    return pascal / 101325.0; // 1 atm = 101325 Pa
}

/// @brief Function to convert Atmosphere to Pascal
/// @param atm -> This is the pressure in Atmosphere
/// @return float -> This returns the pressure in Pascal
float CalcModuleInternals::atmToPascal(float atm)
{
    return atm * 101325.0;
}

/// @brief Function to convert Pascal to Psi
/// @param pascal -> This is the pressure in Pascal
/// @return float -> This returns the pressure in Psi
float CalcModuleInternals::pascalToPsi(float pascal)
{
    return pascal * 0.000145038; // 1 Pa = 0.000145038 psi
}

/// @brief Function to convert Psi to Pascal
/// @param psi -> This is the pressure in Psi
/// @return float -> This returns the pressure in Pascal
float CalcModuleInternals::psiToPascal(float psi)
{
    return psi * 6894.76; // 1 psi = 6894.76 Pa
}

/// @brief Function to calculate the Power
/// @param voltage -> This is the voltage
/// @param current -> This is the current
/// @return float -> This returns the power
float CalcModuleInternals::calculatePower(float voltage, float current)
{
    return voltage * current; // Power (P = V * I)
}

/// @brief Function to caclulate the Current
/// @param voltage -> This is the voltage
/// @param resistance -> This is the resistance
/// @return float -> This returns the current
float CalcModuleInternals::calculateCurrent(float voltage, float resistance)
{
    if (resistance == 0) return 0.0; // Avoid division by zero
    return voltage / resistance; // Current (I = V / R)
}

/// @brief Function to caclulate the Resistance
/// @param voltage -> This is the voltage
/// @param current -> This is the current
/// @return float -> This returns the resistance
float CalcModuleInternals::calculateResistance(float voltage, float current)
{
    if (current == 0) return 0.0; // Avoid division by zero
    return voltage / current; // Resistance (R = V / I)
}

/// @brief Function to extract a float from a getParam String
/// @param response -> The getParam response String
/// @param id -> The compound id to identify how the param string looks like
/// 0 -> Simple GET/SET
/// 1 -> Compound 1
/// 1 -> Compound 2
/// 1 -> Compound 3
/// @return The extracted float from the response
String CalcModuleInternals::extractFloat(String response, int id)
{
	String extracted;

	if (id == 0)
	{
	    int dotIndex = response.lastIndexOf('.');
	    if (dotIndex == -1 || dotIndex < 3) return "0";
	    int startIndex = dotIndex - 3;
	    if (startIndex < 0) startIndex = 0;

	    extracted = response.substring(startIndex, response.length());
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

    return extracted;
}

