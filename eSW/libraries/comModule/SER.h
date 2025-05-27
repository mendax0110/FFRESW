/**
 * @file SerialCommunication.h
 * @brief Handles Serial communication for the comModule
 * @version 0.1
 * @date 2024-09-28
 */

 #ifndef SERIAL_COMMUNICATION_H
 #define SERIAL_COMMUNICATION_H

 #include <Arduino.h>

 namespace comModule
 {
	 /// @brief Class to handle Serial communication \class SerialCommunication
	 class SerialCommunication
	 {
	 public:
		SerialCommunication();
		~SerialCommunication();

		 /**
		  * @brief Function to start the serial communication
		  *
		  * @param baudRate -> The baud rate to use for the serial communication
		  */
		 void beginSerial(long baudRate);

		 /**
		  * @brief Function to end the serial communication
		  *
		  */
		 void endSerial();

		 /**
		  * @brief Function to end the serial communication
		  *
		  * @param data -> The data to send
		  */
		 void sendSerialData(const char* data);

		 /**
		  * @brief Function to receive data over serial
		  *
		  * @param buffer -> The buffer to read the data into
		  * @param length -> The length of the data to read
		  */
		 void receiveSerialData(char* buffer, size_t length);

		 /**
		  * @brief Function to check if the serial communication is initialized
		  *
		  * @return true -> if the serial communication is initialized
		  * @return false -> if the serial communication is not initialized
		  */
		 bool isInitialized() const;

	 private:
		 bool serInitialized = false;
	 };
 }

 #endif // SERIAL_COMMUNICATION_H
