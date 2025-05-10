#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <String.h>
#include <timeModule.h>

/// @brief Class which handle the printed log messages, maps aka parses them and saves them to the SD card. \class LogMapper
class LogManager
{
public:

    /**
     * @brief Get the Instance object
     * 
     * @return LogManager* 
     */
    static LogManager* getInstance();

    /**
     * @brief Function to initialize the SD card.
     *
     * @param cs -> The chip select pin for the SD card.
     */
    void initSDCard(int cs);

    /**
     * @brief Function to shut down the SD card
     */
    void shutdownSDCard();

    /**
     * @brief Function to flush the current Logs in special cases
     */
    void flushLogs();

    /**
     * @brief Function to check if the SD card is initialized.
     * 
     * @return true -> if the SD card is initialized
     * @return false -> if the SD card is not initialized
     */
    bool isSDCardInitialized() const;

    /**
     * @brief Getter for the current time
     * @return The current time as a String
     */
    static String getCurrentTime();

    /**
     * @brief Set the Log File Name object
     * 
     * @param fileName -> The file name to set the log file name to.
     */
    void setLogFileName(const String& fileName);

    /**
     * @brief Function to write a log message to the log file.
     * 
     * @param logMessage -> The log message to write to the log file.
     * @return true -> if the log message was written successfully
     * @return false -> if the log message was not written successfully
     */
    bool writeToLogFile(const String& logMessage);

    /**
     * @brief Function to rename the currently written to file
     *
     * @param oldName -> This is the oldName of the file
     * @param newName -> This is the newName of the file
     */
    void renameFile(const String& oldName, const String& newName);

private:
    LogManager();
    ~LogManager();

    static LogManager* _instance;
    File logFile;
    bool sdCardInitialized = false;
    String logFileName;
    String baseLogFileName;
    
    static const int chipSelectPinEth = 10; // Default CS pin for SD card
    static const int maxLogFileSize = 1024 * 1024 * 100; // 100 MB

    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
};


#endif // LOGMANAGER_H
