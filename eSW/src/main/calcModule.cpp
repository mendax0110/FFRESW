/**
 * @file calcModule.cpp
 * @author Adrian Goessl
 * @brief Implementation of the calcModule class.
 * @version 0.1
 * @date 2024-01-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "includes/calcModule.h"

using namespace calcModule;

calcModuleInternals::calcModuleInternals() {}
calcModuleInternals::~calcModuleInternals() {}

float calcModuleInternals::calculateAverage(float* data, int length)
{
    if (length <= 0) return 0.0;
    float sum = 0.0;
    for (int i = 0; i < length; ++i)
    {
        sum += data[i];
    }
    return sum / length;
}

float calcModuleInternals::findMaximum(float* data, int length)
{
    if (length <= 0) return 0.0;
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

float calcModuleInternals::findMinimum(float* data, int length)
{
    if (length <= 0) return 0.0;
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

float calcModuleInternals::celsiusToFahrenheit(float celsius)
{
    return (celsius * 9.0 / 5.0) + 32.0;
}

float calcModuleInternals::fahrenheitToCelsius(float fahrenheit)
{
    return (fahrenheit - 32.0) * 5.0 / 9.0;
}

float calcModuleInternals::celsiusToKelvin(float celsius)
{
    return celsius + 273.15;
}

float calcModuleInternals::kelvinToCelsius(float kelvin)
{
    return kelvin - 273.15;
}
