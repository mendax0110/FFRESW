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

#ifndef CALCMODULE_H
#define CALCMODULE_H

#include <Arduino.h>

/// @brief Namespace for the calculation module. \namespace calcModule
namespace calcModule
{
    /// @brief Class for the calculation module internals. \class calcModuleInternals
    class CalcModuleInternals
    {
    public:
        CalcModuleInternals();
        ~CalcModuleInternals();

        static float calculateAverage(const float* data, int length);
        static float findMaximum(const float* data, int length);
        static float findMinimum(const float* data, int length);
        static float calculateStandardDeviation(const float* data, int length);
        static float findMedian(float* data, int length);

        static float celsiusToFahrenheit(float celsius);
        static float fahrenheitToCelsius(float fahrenheit);
        static float celsiusToKelvin(float celsius);
        static float kelvinToCelsius(float kelvin);

        static float pascalToAtm(float pascal);
        static float atmToPascal(float atm);
        static float pascalToPsi(float pascal);
        static float psiToPascal(float psi);

        static float calculatePower(float voltage, float current);
        static float calculateCurrent(float voltage, float resistance);
        static float calculateResistance(float voltage, float current);

        static String extractFloat(String response, int id);

    private:
        static void sortArray(float* data, int length);
    };
}

#endif // CALCMODULE_H
