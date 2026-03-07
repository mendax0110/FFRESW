/**
 * @file hal.h
 * @author Adrian Goessl
 * @brief Hardware Abstraction Layer interface for multi-platform support.
 * @version 1.0
 * @date 2026-03-07
 *
 * @copyright Copyright (c) 2026
 *
 * @details Provides a unified C++ interface that abstracts hardware-specific
 *          operations (GPIO, ADC, timers, I2C, SPI, UART, EEPROM, delay…).
 *          Each target platform supplies its own implementation of these
 *          functions so that the application code remains platform-agnostic.
 *
 *          Supported platforms:
 *          - AVR  ATmega2560   (hal_avr.cpp)
 *          - NXP  FRDM-MCXN947 (hal_mcxn947.cpp)
 */
#ifndef HAL_H
#define HAL_H

#include "platform.h"

/// @brief Namespace for the hardware abstraction layer \namespace hal
namespace hal
{
    /* -------------------------------------------------------------- */
    /*  GPIO                                                           */
    /* -------------------------------------------------------------- */

    /// @brief Enum for GPIO pin direction \enum PinMode
    enum class PinMode : uint8_t
    {
        Input,
        Output,
        InputPullup
    };

    /// @brief Enum for digital logic level \enum PinState
    enum class PinState : uint8_t
    {
        Low  = 0,
        High = 1
    };

    /**
     * @brief Configure a GPIO pin direction.
     *
     * @param pin -> The pin identifier.
     * @param mode -> The desired direction / pull configuration.
     */
    void gpioSetMode(pin_t pin, PinMode mode);

    /**
     * @brief Write a digital logic level to a GPIO pin.
     *
     * @param pin -> The pin identifier.
     * @param state -> The desired logic level.
     */
    void gpioWrite(pin_t pin, PinState state);

    /**
     * @brief Read the digital logic level from a GPIO pin.
     *
     * @param pin -> The pin identifier.
     * @return PinState -> The current logic level on the pin.
     */
    PinState gpioRead(pin_t pin);

    /* -------------------------------------------------------------- */
    /*  ADC                                                            */
    /* -------------------------------------------------------------- */

    /**
     * @brief Read a raw value from an analog input channel.
     *
     * @param channel -> The analog channel / pin number.
     * @return uint16_t -> Raw ADC value (range depends on HAL_ADC_RESOLUTION).
     */
    uint16_t adcRead(pin_t channel);

    /**
     * @brief Convert a raw ADC value to voltage.
     *
     * @param rawValue -> The raw ADC reading.
     * @return float -> Voltage in volts.
     */
    float adcToVoltage(uint16_t rawValue);

    /* -------------------------------------------------------------- */
    /*  Timing                                                         */
    /* -------------------------------------------------------------- */

    /**
     * @brief Get the number of milliseconds since system start.
     *
     * @return tick_ms_t -> Millisecond tick count (wraps at ~49 days on 32-bit).
     */
    tick_ms_t millisTick();

    /**
     * @brief Get the number of microseconds since system start.
     *
     * @return tick_us_t -> Microsecond tick count.
     */
    tick_us_t microsTick();

    /**
     * @brief Busy-wait for the specified number of milliseconds.
     *
     * @param ms -> Milliseconds to wait.
     */
    void delayMs(tick_ms_t ms);

    /**
     * @brief Busy-wait for the specified number of microseconds.
     *
     * @param us -> Microseconds to wait.
     */
    void delayUs(tick_us_t us);

    /* -------------------------------------------------------------- */
    /*  PWM / Timer                                                    */
    /* -------------------------------------------------------------- */

    /**
     * @brief Initialise the hardware PWM timer for a given channel.
     *
     * @param channel -> The timer / PWM channel index.
     */
    void pwmInit(uint8_t channel);

    /**
     * @brief Set PWM frequency and duty cycle.
     *
     * @param channel -> The timer / PWM channel index.
     * @param frequencyHz -> Desired frequency in Hz.
     * @param dutyPercent -> Desired duty cycle 0 – 100.
     */
    void pwmSetFrequencyAndDuty(uint8_t channel, uint32_t frequencyHz, uint8_t dutyPercent);

    /**
     * @brief Stop PWM output on a channel.
     *
     * @param channel -> The timer / PWM channel index.
     */
    void pwmStop(uint8_t channel);

    /* -------------------------------------------------------------- */
    /*  UART / Serial                                                  */
    /* -------------------------------------------------------------- */

    /**
     * @brief Initialise a UART peripheral.
     *
     * @param port -> The UART port index (0 = default debug port).
     * @param baudRate -> Desired baud rate.
     */
    void uartInit(uint8_t port, uint32_t baudRate);

    /**
     * @brief Write a byte buffer to UART.
     *
     * @param port -> The UART port index.
     * @param data -> Pointer to the data buffer.
     * @param length -> Number of bytes to send.
     */
    void uartWrite(uint8_t port, const uint8_t* data, size_t length);

    /**
     * @brief Write a null-terminated string to UART.
     *
     * @param port -> The UART port index.
     * @param str -> The string to send.
     */
    void uartPrint(uint8_t port, const char* str);

    /**
     * @brief Write a string followed by CRLF to UART.
     *
     * @param port -> The UART port index.
     * @param str -> The string to send.
     */
    void uartPrintln(uint8_t port, const char* str);

    /**
     * @brief Check how many bytes are available to read from UART.
     *
     * @param port -> The UART port index.
     * @return size_t -> Number of bytes available.
     */
    size_t uartAvailable(uint8_t port);

    /**
     * @brief Read a single byte from UART (blocking).
     *
     * @param port -> The UART port index.
     * @return int -> The byte read, or -1 if nothing available.
     */
    int uartRead(uint8_t port);

    /**
     * @brief Check if the UART port is ready / connected.
     *
     * @param port -> The UART port index.
     * @return bool -> true if the port is operational.
     */
    bool uartReady(uint8_t port);

    /**
     * @brief Shut down a UART peripheral.
     *
     * @param port -> The UART port index.
     */
    void uartEnd(uint8_t port);

    /* -------------------------------------------------------------- */
    /*  I2C                                                            */
    /* -------------------------------------------------------------- */

    /**
     * @brief Initialise the I2C bus.
     */
    void i2cInit();

    /**
     * @brief Write data to an I2C slave device.
     *
     * @param address -> 7-bit slave address.
     * @param data -> Pointer to the data buffer.
     * @param length -> Number of bytes to write.
     * @return bool -> true on success.
     */
    bool i2cWrite(uint8_t address, const uint8_t* data, size_t length);

    /**
     * @brief Read data from an I2C slave device.
     *
     * @param address -> 7-bit slave address.
     * @param buffer -> Pointer to the receive buffer.
     * @param length -> Number of bytes to read.
     * @return bool -> true on success.
     */
    bool i2cRead(uint8_t address, uint8_t* buffer, size_t length);

    /**
     * @brief Shut down the I2C bus.
     */
    void i2cEnd();

    /* -------------------------------------------------------------- */
    /*  SPI                                                            */
    /* -------------------------------------------------------------- */

    /**
     * @brief Initialise the SPI bus.
     */
    void spiInit();

    /**
     * @brief Transfer a single byte over SPI (full-duplex).
     *
     * @param data -> Byte to send.
     * @return uint8_t -> Byte received.
     */
    uint8_t spiTransfer(uint8_t data);

    /**
     * @brief Shut down the SPI bus.
     */
    void spiEnd();

    /* -------------------------------------------------------------- */
    /*  Non-volatile storage (EEPROM / Flash)                          */
    /* -------------------------------------------------------------- */

    /**
     * @brief Write a block of data to non-volatile storage.
     *
     * @param address -> Start address in NV storage.
     * @param data -> Pointer to the data to write.
     * @param length -> Number of bytes.
     * @return bool -> true on success.
     */
    bool nvWrite(uint16_t address, const void* data, size_t length);

    /**
     * @brief Read a block of data from non-volatile storage.
     *
     * @param address -> Start address in NV storage.
     * @param data -> Pointer to the receive buffer.
     * @param length -> Number of bytes.
     * @return bool -> true on success.
     */
    bool nvRead(uint16_t address, void* data, size_t length);

    /* -------------------------------------------------------------- */
    /*  System                                                         */
    /* -------------------------------------------------------------- */

    /**
     * @brief Perform a software reset of the MCU.
     */
    void systemReset();

    /**
     * @brief Enable the hardware watchdog timer.
     *
     * @param timeoutMs -> Watchdog timeout in milliseconds.
     */
    void watchdogEnable(uint16_t timeoutMs);

    /**
     * @brief Disable the hardware watchdog timer.
     */
    void watchdogDisable();

    /**
     * @brief Feed / kick the hardware watchdog timer.
     */
    void watchdogReset();

    /**
     * @brief Get the amount of free heap/SRAM in bytes.
     *
     * @return size_t -> Free memory in bytes.
     */
    size_t getFreeHeap();

    /**
     * @brief Get the total SRAM / RAM size in bytes.
     *
     * @return size_t -> Total RAM in bytes.
     */
    size_t getTotalRam();

} // namespace hal

#endif // HAL_H
