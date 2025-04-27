/**
 * @file calcModule.h
 * @author Adrian Goessl
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

        /**
         * @brief Function to calculate the average of a data set.
         *
         * @param data -> The data set to calculate the average from.
         * @param length -> The length of the data set.
         * @return float -> The average of the data set.
         */
        static float calculateAverage(const float* data, int length);

        /**
         * @brief Function to calculate the maximum value of a data set.
         *
         * @param data -> The data set to calculate the maximum value from.
         * @param length -> The length of the data set.
         * @return float -> The maximum value of the data set.
         */
        static float findMaximum(const float* data, int length);

        /**
         * @brief Function to calculate the minimum value of a data set.
         *
         * @param data -> The data set to calculate the minimum value from.
         * @param length -> The length of the data set.
         * @return float -> The minimum value of the data set.
         */
        static float findMinimum(const float* data, int length);

        /**
         * @brief Function to calculate the standard deviation of a data set.
         *
         * @param data -> The data set to calculate the standard deviation from.
         * @param length -> The length of the data set.
         * @return float -> The standard deviation of the data set.
         */
        static float calculateStandardDeviation(const float* data, int length);

        /**
         * @brief Function to calculate the median of a data set.
         *
         * @param data -> The data set to calculate the median from.
         * @param length -> The length of the data set.
         * @return float -> The median of the data set.
         */
        static float findMedian(float* data, int length);

        /**
         * @brief Function to convert celsius to fahrenheit.
         *
         * @param celsius -> The temperature in celsius.
         * @return float -> The temperature in fahrenheit.
         */
        static float celsiusToFahrenheit(float celsius);

        /**
         * @brief Function to convert fahrenheit to celsius.
         *
         * @param fahrenheit -> The temperature in fahrenheit.
         * @return float -> The temperature in celsius.
         */
        static float fahrenheitToCelsius(float fahrenheit);

        /**
         * @brief Function to convert celsius to kelvin.
         *
         * @param celsius -> The temperature in celsius.
         * @return float -> The temperature in kelvin.
         */
        static float celsiusToKelvin(float celsius);

        /**
         * @brief Function to convert kelvin to celsius.
         *
         * @param kelvin -> The temperature in kelvin.
         * @return float -> The temperature in celsius.
         */
        static float kelvinToCelsius(float kelvin);

        /**
         * @brief Function to convert pascal to atm.
         *
         * @param pascal -> The pressure in pascal.
         * @return float -> The pressure in atm.
         */
        static float pascalToAtm(float pascal);

        /**
         * @brief Function to convert atm to pascal.
         *
         * @param atm -> The pressure in atm.
         * @return float -> The pressure in pascal.
         */
        static float atmToPascal(float atm);

        /**
         * @brief Function to convert pascal to psi.
         *
         * @param pascal -> The pressure in pascal.
         * @return float -> The pressure in psi.
         */
        static float pascalToPsi(float pascal);

        /**
         * @brief Function to convert psi to pascal.
         *
         * @param psi -> The pressure in psi.
         * @return float -> The pressure in pascal.
         */
        static float psiToPascal(float psi);

        /**
         * @brief Function to calculate the power.
         *
         * @param voltage -> The voltage.
         * @param current -> The current.
         * @return float -> The calculated power.
         */
        static float calculatePower(float voltage, float current);

        /**
         * @brief Funcion to calculate the current.
         *
         * @param voltage -> The voltage.
         * @param resistance -> The resistance.
         * @return float -> The calculated current.
         */
        static float calculateCurrent(float voltage, float resistance);

        /**
         * @brief Function to caculate the Resistance.
         *
         * @param voltage -> The voltage.
         * @param current -> The current.
         * @return float -> The calculated resistance.
         */
        static float calculateResistance(float voltage, float current);

        /**
         * @brief Extract the float from a VAT uC eth frame.
         *
         * @param response -> The response from the VAT uC.
         * @param id -> The id of the compound.
         *
         *  0 -> Simple GET/SET
         *	1 -> Compound 1
         *	1 -> Compound 2
         *	1 -> Compound 3
         *
         * @return float -> The extracted float.
         */
        static float extractFloat(String response, int id);

    private:

        /**
         * @brief Function to sort an array.
         *
         * @param data -> The data array to sort.
         * @param length -> The length of the data array.
         */
        static void sortArray(float* data, int length);

        /**
         * @brief Function to round a given value with given precision
         *
         * @param value -> the value we want to round
         * @param precision -> how many numbers after the decimal point
         */
        static float roundToPrecision(float value, int precision);
    };
}

#endif // CALCMODULE_H
