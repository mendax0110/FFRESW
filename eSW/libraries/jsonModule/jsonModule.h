/**
 * @file jsonModule.h
 * @author Adrian Goessl
 * @brief 
 * @version 0.1
 * @date 2024-09-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef JSONMODULE_H
#define JSONMODULE_H

#define _STL_NOEXCEPT

#ifndef ARDUINO_STL_MEMORY
#define ARDUINO_STL_MEMORY 0  // Disable STL memory management (new, delete, new_handler)
#endif

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoSTL.h>
#include <map>

/// @brief Namespace for the JSON module. \namespace jsonModule
namespace jsonModule
{
    /// @brief Class for the JSON module internals. \class jsonModuleInternals
    class JsonModuleInternals
    {
    public:
        JsonModuleInternals();
        ~JsonModuleInternals();

        /**
         * @brief Create a Json object with a key and a value.
         *
         * @param key -> The key of the Json object
         * @param value -> The value of the Json object
         */
        void createJson(const char* key, const char* value);

        /**
         * @brief Create a Json Float object
         *
         * @param key -> The key of the Json object
         * @param value -> The value of the Json object
         */
        void createJsonFloat(const char* key, float value);

        /**
         * @brief Create a Json Int object
         *
         * @param key -> The key of the Json object
         * @param value -> The value of the Json object
         */
        void createJsonInt(const char* key, int value);

        /**
         * @brief Create a Json String object
         *
         * @param key -> The key of the Json object
         * @param value -> The value of the Json object
         */
        void createJsonString(const char* key, String& value);

        /**
         * @brief Create a Json String Const object
         *
         * @param key -> The key of the Json object
         * @param value -> The value of the Json object
         */
        void createJsonStringConst(const char* key, const String& value);

        /**
         * @brief Function to send the Json object over the Serial connection.
         *
         */
        void sendJsonSerial();

        /**
         * @brief Function to send the Json object over the Ethernet connection.
         *
         * @param endpoint -> The endpoint to send the Json object to
         */
        void sendJsonEthernet(const char* endpoint);

        /**
         * @brief Get the Json String object
         *
         * @return String -> The Json String object
         */
        String getJsonString() const;


        /**
         * @brief Map the Json object to a map of Strings and floats.
         *
         * https://github.com/mike-matera/ArduinoSTL/issues/84
         *
         * @param rawJson -> The raw Json object
         * @return std::map<String, float> -> The mapped Json object
         */
        std::map<String, float> mapJsonToDoubles(const String& rawJson);

        /**
         * @brief Clear the Json object.
         *
         */
        void clearJson();

        /**
         * @brief Prints information about the Json object.
         *
         */
        void printJsonDocMemory();

        size_t jsonBuffer;

    private:
        StaticJsonDocument<512> jsonDoc;

    };
}

#endif // JSONMODULE_H
