/**
 * @file calcModule.h
 * @author your name (you@domain.com)
 * @brief Header file for the calculation module handling sensor data.
 * @version 0.1
 * @date 2024-01-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <Arduino.h>
#include <math.h>

#ifndef CALCMODULE_H
#define CALCMODULE_H

/// @brief This is the namespace for the calcModule
namespace calcModule
{
    /// @brief This is the class for the calcModule
    class calcModuleInternals
    {
    public:
        calcModuleInternals();
        ~calcModuleInternals();

        float calculateAverage(float* data, int length);
        float findMaximum(float* data, int length);
        float findMinimum(float* data, int length);
        float celsiusToFahrenheit(float celsius);
        float fahrenheitToCelsius(float fahrenheit);
        float celsiusToKelvin(float celsius);
        float kelvinToCelsius(float kelvin);

    private:

    protected:
    };
}

#endif // CALCMODULE_H
