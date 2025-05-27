/**
 * @file serialMenu.h
 * @author Adrian Goessl
 * @brief Header file for the serial menu handling serial menu interaction, logging...
 * @version 0.1
 * @date 2024-01-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef SERIAL_MENU_H
#define SERIAL_MENU_H

#include <Arduino.h>
#include <frt.h>
#include <timeModule.h>


/// @brief Serial menu structure \struct MenuItem
struct MenuItem
{
    const char* label;
    char key;
    void (*callback)();
};

/// @brief Class for the serial menu. \class SerialMenu
class SerialMenu
{
public:

	/// @brief Enum Class for the differnet Outputlevels \class Outputlevel
	enum class OutputLevel
	{
	    DEBUG,
	    INFO,
	    WARNING,
	    ERROR,
	    CRITICAL,
	    STATUS,
	    PLAIN
	};

	SerialMenu();
	~SerialMenu();

    /**
     * @brief Function to load the menu items.
     *
     * @param items -> The menu items.
     * @param size ->  The size of the menu items.
     */
    void load(MenuItem* items, size_t size);

    /**
     * @brief Function to show the menu.
     *
     */
    void show();

    /**
     * @brief Function to run the menu.
     *
     */
    void run();

    /**
     * @brief Function to print a message to the serial port, using mutexes, output level and new line options.
     *
     * @param level -> The output level of the message.
     * @param message -> The message to print, a String object.
     * @param newLine -> Whether to add a new line at the end of the message.
     */
    static void printToSerial(OutputLevel level, const String& message, bool newLine = true, bool logMessage = false);

    /**
     * @brief Function to print a message to the serial port, using mutexes, output level and new line options.
     *
     * @param level -> The output level of the message.
     * @param message -> The message to print, a __FlashStringHelper pointer.
     * @param newLine -> Whether to add a new line at the end of the message.
     */
    static void printToSerial(OutputLevel level, const __FlashStringHelper* message, bool newLine = true, bool logMessage = false);

    /**
     * @brief Funtion to print a message to the serial port, using mutexes, output level and new line options.
     *
     * @param message -> The message to print, a String object.
     * @param newLine -> Whether to add a new line at the end of the message.
     */
    static void printToSerial(const String& message, bool newLine = true, bool logMessage = false);

    /**
     * @brief Function to print a message to the serial port, using mutexes, output level and new line options.
     *
     * @param message -> The message to print, a __FlashStringHelper pointer.
     * @param newLine -> Whether to add a new line at the end of the message.
     */
    static void printToSerial(const __FlashStringHelper* message, bool newLine = true, bool logMessage = false);

    /**
     * @brief Getter for the current time
     * @return The current time as a String
     */
    static String getCurrentTime();

private:
    MenuItem* currentMenu;
    size_t menuSize;
};

#endif // SERIAL_MENU_H
