#include <logManager.h>
#include <ptrUtils.h>

LogManager* LogManager::_instance = nullptr;

Sd2Card card;
SdVolume volume;
SdFile root;

LogManager::LogManager() : sdCardInitialized(false)
{

}

LogManager::~LogManager()
{
    shutdownSDCard();

    sdCardInitialized = false;
}

LogManager* LogManager::getInstance()
{
    if (PtrUtils::IsNullPtr(_instance))
    {
        _instance = new LogManager();
    }
    return _instance;
}

void LogManager::initSDCard(int cs)
{
	// WHY ALL THIS? -> https://stackoverflow.com/questions/17503094/arduinio-sd-on-ethernet-shield-not-working-at-all
    pinMode(cs, OUTPUT);
    digitalWrite(cs, HIGH); // Disable SD-Card
    pinMode(chipSelectPinEth, OUTPUT);
    digitalWrite(chipSelectPinEth, HIGH);  // Disable Ethernet

    if (!card.init(SPI_HALF_SPEED, cs))
    {
    	digitalWrite(chipSelectPinEth, LOW);  // Re-enable Ethernet before returning
        sdCardInitialized = false;
        return;
    }

    if (!volume.init(card))
    {
    	digitalWrite(chipSelectPinEth, LOW);  // Re-enable Ethernet before returning
        sdCardInitialized = false;
        return;
    }

    // Re-enable Ethernet after SD initialization
    digitalWrite(chipSelectPinEth, LOW);
    sdCardInitialized = true;
}

void LogManager::shutdownSDCard()
{
    if (logFile)
    {
        logFile.close();
    }
    else
    {
    	SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, "Failed to close currently opened logFile: " + logFile);
    }

	if (isSDCardInitialized() &&
				!card.isBusy() &&
				!root.isOpen())
	{
		SD.end();
		sdCardInitialized = false;
	}
	else
	{
		SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, F("Failed to gracefully shutdown MicroSD Card."));
	}
}

bool LogManager::isSDCardInitialized() const
{
    return sdCardInitialized;
}

String LogManager::getCurrentTime()
{
    using namespace timeModule;
    TimeModuleInternals* timeInstance = TimeModuleInternals::getInstance();

    if (timeInstance == nullptr)
    {
        return "0000-00-00T00:00:00Z";
    }

    return TimeModuleInternals::formatTimeString(timeInstance->getSystemTime());
}

void LogManager::setLogFileName(const String& baseName)
{
    String timeStamp = getCurrentTime();
    timeStamp.replace(":", "-");
    timeStamp.replace("T", "_");

    baseLogFileName = timeStamp + "_" + baseName;
    logFileName = baseLogFileName;
}

void LogManager::renameFile(const String& oldName, const String& newName)
{
    pinMode(chipSelectPinEth, OUTPUT);            // Ensure CS pin is output
    digitalWrite(chipSelectPinEth, HIGH);         // Disable W5100

    File oldFile = SD.open(oldName);
    if (!oldFile)
    {
        digitalWrite(chipSelectPinEth, LOW);  // Re-enable Ethernet before returning
        return;
    }

    File newFile = SD.open(newName, FILE_WRITE);
    if (!newFile)
    {
        oldFile.close();
        digitalWrite(chipSelectPinEth, LOW);  // Re-enable Ethernet before returning
        return;
    }

    while (oldFile.available())
    {
        newFile.write(oldFile.read());
    }

    oldFile.close();
    newFile.close();

    SD.remove(oldName);
    digitalWrite(chipSelectPinEth, LOW);  // Enable W5100 (Ethernet)
}

bool LogManager::writeToLogFile(const String& logMessage)
{
    if (!isSDCardInitialized())
    {
        return false;
    }

    pinMode(chipSelectPinEth, OUTPUT);            // Ensure CS pin is output
    digitalWrite(chipSelectPinEth, HIGH);         // Disable W5100

    if (logFile)
    {
        logFile.close();
    }

    logFile = SD.open(logFileName, FILE_WRITE);
    if (!logFile)
    {
        digitalWrite(chipSelectPinEth, LOW);  // Enable W5100 (Ethernet)
        return false;
    }

    if (logFile.size() > maxLogFileSize)
    {
        logFile.close();

        int index = 1;
        String newFileName;
        do
        {
            newFileName = baseLogFileName;
            int dotIndex = newFileName.lastIndexOf(".");
            if (dotIndex != -1)
            {
                newFileName = newFileName.substring(0, dotIndex) + "_" + String(index++) + newFileName.substring(dotIndex);
            }
            else
            {
                newFileName += "_" + String(index++);
            }
        } while (SD.exists(newFileName));

        renameFile(logFileName, newFileName);

        logFile = SD.open(logFileName, FILE_WRITE);
        if (!logFile)
        {
            digitalWrite(10, LOW);  // Enable W5100 (Ethernet)
            return false;
        }
    }

    logFile.println(logMessage);
    logFile.flush();
    logFile.close();

    digitalWrite(chipSelectPinEth, LOW);  // Enable W5100 (Ethernet)

    return true;
}

void LogManager::flushLogs()
{
	if (logFile)
	{
		logFile.flush();
	}
	else
	{
		SerialMenu::printToSerial(SerialMenu::OutputLevel::ERROR, "No log file currently open to flush.");
	}
}
