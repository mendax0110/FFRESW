/**
 * @file jsonModule.h
 * @author Adrian Goessl
 * @brief Header for the JsonModuleInternals class.
 * @version 0.2
 * @date 2025-05-18
 * 
 * @copyright Copyright (c) 2025
 */
#ifndef JSONMODULE_H
#define JSONMODULE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <serialMenu.h>

/// @brief Namespace for the JSON module. \namespace jsonModule
namespace jsonModule
{
    /// @brief Class for the JSON module internals. \class JsonModuleInternals
    class JsonModuleInternals
    {
    public:
        JsonModuleInternals();
        ~JsonModuleInternals();

        /**
         * @brief Add a key-value pair to the Json document.
         *
         * @tparam T Type of the value.
         * @param key Key to set.
         * @param value Value to associate with the key.
         */
        template<typename T>
        void createJson(const char* key, T value)
        {
            if (!jsonDoc[key].set(value))
            {
                SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Failed to set JSON key."));
            }
        }

        /**
         * @brief Add a key-array pair to the Json document.
         *
         * @tparam T Type of the array elements.
         * @tparam N Size of the array (inferred automatically).
         * @param key Key to associate with the array.
         * @param values C-style array of values to add under the key.
         */
        template<typename T, size_t N>
        void createJsonArray(const char* key, const T (&values)[N])
        {
            JsonArray array = jsonDoc.createNestedArray(key);
            for (size_t i = 0; i < N; ++i)
            {
                array.add(values[i]);
            }
        }

        /**
         * @brief Function to send the Json object over the Serial connection.
         */
        void sendJsonSerial();

        /**
         * @brief Get the Json String object.
         *
         * @return String -> The Json String object.
         */
        String getJsonString() const;

        /**
         * @brief Clear the Json object.
         */
        void clearJson();

        /**
         * @brief Prints information about the Json object.
         */
        void printJsonDocMemory();

        /**
         * @brief Check if there is enough capacity left in the JsonDocument.
         *
         * @param additionalSize Approximate size of the data to be added.
         * @return true If there is enough space.
         * @return false If adding may overflow the buffer.
         */
        bool hasCapacityFor(size_t additionalSize) const;

        size_t jsonBufferSize;

    private:
        StaticJsonDocument<512> jsonDoc;
    };
}

#endif // JSONMODULE_H
