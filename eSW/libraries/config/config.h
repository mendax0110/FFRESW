// config.h
// FreeROTS Kernel Configuration

#ifndef CONFIG_H
#define CONFIG_H

// Enable Arduino C++ Interface
// This allows the HeliOS kernel to interact with the Arduino API
#define CONFIG_ENABLE_ARDUINO_CPP_INTERFACE

// Enable System Assertions (optional, for debugging purposes)
#define CONFIG_ENABLE_SYSTEM_ASSERT
#define CONFIG_SYSTEM_ASSERT_BEHAVIOR(file, line) __ArduinoAssert__(file, line)

// Message Queue Configuration
#define CONFIG_MESSAGE_VALUE_BYTES 0x8u  // Message queue message value size in bytes

// Task Notification Configuration
#define CONFIG_NOTIFICATION_VALUE_BYTES 0x8u  // Task notification value size in bytes

// Task Name Configuration
#define CONFIG_TASK_NAME_BYTES 0x8u  // Length of task names in bytes

// Memory Region Configuration
#define CONFIG_MEMORY_REGION_SIZE_IN_BLOCKS 0x10u  // Number of memory blocks (16 blocks)
#define CONFIG_MEMORY_REGION_BLOCK_SIZE 0x20u  // Memory block size in bytes (32 bytes)

// Queue Configuration
#define CONFIG_QUEUE_MINIMUM_LIMIT 0x5u  // Minimum queue size limit (5 items)

// Stream Buffer Configuration
#define CONFIG_STREAM_BUFFER_BYTES 0x20u  // Stream buffer length (32 bytes)

// Task Watchdog Timer
#define CONFIG_TASK_WD_TIMER_ENABLE  // Enable watchdog timer for tasks

// Device Name Configuration
#define CONFIG_DEVICE_NAME_BYTES 0x8u  // Device name length (8 bytes)

#endif // CONFIG_H
