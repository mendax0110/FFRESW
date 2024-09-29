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

using namespace calcModule;

calcModuleInternals::calcModuleInternals() {}
calcModuleInternals::~calcModuleInternals() {}

/// @brief Function to calcuate the Average of the given data
/// @param data -> This is the data array
/// @param length -> This is the length of the data array
/// @return float -> This returns the average of the data
float calcModuleInternals::calculateAverage(const float* data, int length) const
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
float calcModuleInternals::findMaximum(const float* data, int length) const
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
float calcModuleInternals::findMinimum(const float* data, int length) const
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
float calcModuleInternals::calculateStandardDeviation(const float* data, int length) const
{
    if (length <= 0 || data == nullptr) return 0.0;
    float mean = calculateAverage(data, length);
    float variance = 0.0;
    for (int i = 0; i < length; ++i)
    {
        variance += (data[i] - mean) * (data[i] - mean);
    }
    variance /= length;
    return sqrt(variance);
}

/// @brief Function to find the Median of the given data
/// @param data -> This is the data array
/// @param length -> This is the length of the data array
/// @return float -> This returns the median of the data
float calcModuleInternals::findMedian(float* data, int length) const
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
void calcModuleInternals::sortArray(float* data, int length) const
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
float calcModuleInternals::celsiusToFahrenheit(float celsius) const
{
    return (celsius * 9.0 / 5.0) + 32.0;
}

/// @brief Function to convert Fahrenheit to Celsius
/// @param fahrenheit -> This is the temperature in Fahrenheit
/// @return float -> This returns the temperature in Celsius
float calcModuleInternals::fahrenheitToCelsius(float fahrenheit) const
{
    return (fahrenheit - 32.0) * 5.0 / 9.0;
}

/// @brief Function to convert Celsius to Kelvin
/// @param celsius -> This is the temperature in Celsius
/// @return float -> This returns the temperature in Kelvin
float calcModuleInternals::celsiusToKelvin(float celsius) const
{
    return celsius + 273.15;
}

/// @brief Function to convert Kelvin to Celsius
/// @param kelvin -> This is the temperature in Kelvin
/// @return float -> This returns the temperature in Celsius
float calcModuleInternals::kelvinToCelsius(float kelvin) const
{
    return kelvin - 273.15;
}

/// @brief Function to convert Pascal to Atmosphere
/// @param pascal -> This is the pressure in Pascal
/// @return float -> This returns the pressure in Atmosphere
float calcModuleInternals::pascalToAtm(float pascal) const
{
    return pascal / 101325.0; // 1 atm = 101325 Pa
}

/// @brief Function to convert Atmosphere to Pascal
/// @param atm -> This is the pressure in Atmosphere
/// @return float -> This returns the pressure in Pascal
float calcModuleInternals::atmToPascal(float atm) const
{
    return atm * 101325.0;
}

/// @brief Function to convert Pascal to Psi
/// @param pascal -> This is the pressure in Pascal
/// @return float -> This returns the pressure in Psi
float calcModuleInternals::pascalToPsi(float pascal) const
{
    return pascal * 0.000145038; // 1 Pa = 0.000145038 psi
}

/// @brief Function to convert Psi to Pascal
/// @param psi -> This is the pressure in Psi
/// @return float -> This returns the pressure in Pascal
float calcModuleInternals::psiToPascal(float psi) const
{
    return psi * 6894.76; // 1 psi = 6894.76 Pa
}

/// @brief Function to calculate the Power
/// @param voltage -> This is the voltage
/// @param current -> This is the current
/// @return float -> This returns the power
float calcModuleInternals::calculatePower(float voltage, float current) const
{
    return voltage * current; // Power (P = V * I)
}

/// @brief Function to caclulate the Current
/// @param voltage -> This is the voltage
/// @param resistance -> This is the resistance
/// @return float -> This returns the current
float calcModuleInternals::calculateCurrent(float voltage, float resistance) const
{
    if (resistance == 0) return 0.0; // Avoid division by zero
    return voltage / resistance; // Current (I = V / R)
}

/// @brief Function to caclulate the Resistance
/// @param voltage -> This is the voltage
/// @param current -> This is the current
/// @return float -> This returns the resistance
float calcModuleInternals::calculateResistance(float voltage, float current) const
{
    if (current == 0) return 0.0; // Avoid division by zero
    return voltage / current; // Resistance (R = V / I)
}
