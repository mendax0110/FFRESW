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

        float calculateAverage(const float* data, int length) const;
        float findMaximum(const float* data, int length) const;
        float findMinimum(const float* data, int length) const;
        float calculateStandardDeviation(const float* data, int length) const;
        float findMedian(float* data, int length) const;

        float celsiusToFahrenheit(float celsius) const;
        float fahrenheitToCelsius(float fahrenheit) const;
        float celsiusToKelvin(float celsius) const;
        float kelvinToCelsius(float kelvin) const;

        float pascalToAtm(float pascal) const;
        float atmToPascal(float atm) const;
        float pascalToPsi(float pascal) const;
        float psiToPascal(float psi) const;

        float calculatePower(float voltage, float current) const; // Power (P = V * I)
        float calculateCurrent(float voltage, float resistance) const; // Current (I = V / R)
        float calculateResistance(float voltage, float current) const; // Resistance (R = V / I)

    private:
        void sortArray(float* data, int length) const;
    };
}

#endif // CALCMODULE_H
