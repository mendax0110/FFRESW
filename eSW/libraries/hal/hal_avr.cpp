/**
 * @file hal_avr.cpp
 * @author Adrian Goessl
 * @brief HAL implementation for AVR ATmega2560 (Arduino).
 * @version 1.0
 * @date 2026-03-07
 *
 * @copyright Copyright (c) 2026
 *
 * @details Maps the abstract HAL interface to the Arduino / AVR API.
 *          This file is only compiled when PLATFORM_AVR is defined.
 */

#include "hal.h"

#if defined(PLATFORM_AVR)

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>
#include <avr/wdt.h>

/* ------------------------------------------------------------------ */
/*  Helpers                                                            */
/* ------------------------------------------------------------------ */

extern unsigned int __heap_start;
extern void* __brkval;

/* ------------------------------------------------------------------ */
/*  GPIO                                                               */
/* ------------------------------------------------------------------ */

void hal::gpioSetMode(pin_t pin, PinMode mode)
{
    switch (mode)
    {
        case PinMode::Input:
            pinMode(static_cast<uint8_t>(pin), INPUT);
            break;
        case PinMode::Output:
            pinMode(static_cast<uint8_t>(pin), OUTPUT);
            break;
        case PinMode::InputPullup:
            pinMode(static_cast<uint8_t>(pin), INPUT_PULLUP);
            break;
    }
}

void hal::gpioWrite(pin_t pin, PinState state)
{
    digitalWrite(static_cast<uint8_t>(pin), state == PinState::High ? HIGH : LOW);
}

hal::PinState hal::gpioRead(pin_t pin)
{
    return digitalRead(static_cast<uint8_t>(pin)) == HIGH
        ? PinState::High
        : PinState::Low;
}

/* ------------------------------------------------------------------ */
/*  ADC                                                                */
/* ------------------------------------------------------------------ */

uint16_t hal::adcRead(pin_t channel)
{
    return static_cast<uint16_t>(analogRead(static_cast<uint8_t>(channel)));
}

float hal::adcToVoltage(uint16_t rawValue)
{
    return static_cast<float>(rawValue) * (HAL_VCC / static_cast<float>(HAL_ADC_MAX_VALUE));
}

/* ------------------------------------------------------------------ */
/*  Timing                                                             */
/* ------------------------------------------------------------------ */

tick_ms_t hal::millisTick()
{
    return static_cast<tick_ms_t>(millis());
}

tick_us_t hal::microsTick()
{
    return static_cast<tick_us_t>(micros());
}

void hal::delayMs(tick_ms_t ms)
{
    delay(ms);
}

void hal::delayUs(tick_us_t us)
{
    delayMicroseconds(us);
}

/* ------------------------------------------------------------------ */
/*  PWM / Timer                                                        */
/* ------------------------------------------------------------------ */

void hal::pwmInit(uint8_t channel)
{
    (void)channel;
    // Timer1 Fast PWM, no prescaler (16 MHz clock)
    TCCR1A = (1 << COM1A1) | (0 << COM1A0) |
             (1 << COM1B1) | (1 << COM1B0) |
             (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) |
             (1 << CS10);
}

void hal::pwmSetFrequencyAndDuty(uint8_t channel, uint32_t frequencyHz, uint8_t dutyPercent)
{
    (void)channel;
    if (frequencyHz == 0)
    {
        return;
    }
    uint16_t top = static_cast<uint16_t>(HAL_CLOCK_HZ / frequencyHz);
    uint16_t ocrValue = static_cast<uint16_t>((static_cast<uint32_t>(top) * dutyPercent) / 100);
    ICR1 = top;
    OCR1A = ocrValue;
    OCR1B = ocrValue;
}

void hal::pwmStop(uint8_t channel)
{
    (void)channel;
    TCCR1A = 0;
    TCCR1B = 0;
    OCR1A = 0;
    OCR1B = 0;
}

/* ------------------------------------------------------------------ */
/*  UART / Serial                                                      */
/* ------------------------------------------------------------------ */

static HardwareSerial* getSerialPort(uint8_t port)
{
    switch (port)
    {
        case 1:  return &Serial1;
        case 2:  return &Serial2;
        case 3:  return &Serial3;
        default: return &Serial;
    }
}

void hal::uartInit(uint8_t port, uint32_t baudRate)
{
    getSerialPort(port)->begin(baudRate);
}

void hal::uartWrite(uint8_t port, const uint8_t* data, size_t length)
{
    getSerialPort(port)->write(data, length);
}

void hal::uartPrint(uint8_t port, const char* str)
{
    getSerialPort(port)->print(str);
}

void hal::uartPrintln(uint8_t port, const char* str)
{
    getSerialPort(port)->println(str);
}

size_t hal::uartAvailable(uint8_t port)
{
    return static_cast<size_t>(getSerialPort(port)->available());
}

int hal::uartRead(uint8_t port)
{
    return getSerialPort(port)->read();
}

bool hal::uartReady(uint8_t port)
{
    return static_cast<bool>(*getSerialPort(port));
}

void hal::uartEnd(uint8_t port)
{
    getSerialPort(port)->end();
}

/* ------------------------------------------------------------------ */
/*  I2C                                                                */
/* ------------------------------------------------------------------ */

void hal::i2cInit()
{
    Wire.begin();
}

bool hal::i2cWrite(uint8_t address, const uint8_t* data, size_t length)
{
    Wire.beginTransmission(address);
    Wire.write(data, length);
    return Wire.endTransmission() == 0;
}

bool hal::i2cRead(uint8_t address, uint8_t* buffer, size_t length)
{
    size_t received = Wire.requestFrom(address, static_cast<uint8_t>(length));
    if (received != length)
    {
        return false;
    }
    for (size_t i = 0; i < length; ++i)
    {
        buffer[i] = static_cast<uint8_t>(Wire.read());
    }
    return true;
}

void hal::i2cEnd()
{
    // AVR Wire library has no end() in all versions; safe no-op
}

/* ------------------------------------------------------------------ */
/*  SPI                                                                */
/* ------------------------------------------------------------------ */

void hal::spiInit()
{
    SPI.begin();
}

uint8_t hal::spiTransfer(uint8_t data)
{
    return SPI.transfer(data);
}

void hal::spiEnd()
{
    SPI.end();
}

/* ------------------------------------------------------------------ */
/*  Non-volatile storage (EEPROM)                                      */
/* ------------------------------------------------------------------ */

bool hal::nvWrite(uint16_t address, const void* data, size_t length)
{
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    for (size_t i = 0; i < length; ++i)
    {
        EEPROM.update(address + i, bytes[i]);
    }
    return true;
}

bool hal::nvRead(uint16_t address, void* data, size_t length)
{
    uint8_t* bytes = static_cast<uint8_t*>(data);
    for (size_t i = 0; i < length; ++i)
    {
        bytes[i] = EEPROM.read(address + i);
    }
    return true;
}

/* ------------------------------------------------------------------ */
/*  System                                                             */
/* ------------------------------------------------------------------ */

void hal::systemReset()
{
    wdt_enable(WDTO_15MS);
    while (true) { }
}

void hal::watchdogEnable(uint16_t timeoutMs)
{
    uint8_t wdtValue;
    if (timeoutMs <= 15)
    {
        wdtValue = WDTO_15MS;
    }
    else if (timeoutMs <= 30)
    {
        wdtValue = WDTO_30MS;
    }
    else if (timeoutMs <= 60)
    {
        wdtValue = WDTO_60MS;
    }
    else if (timeoutMs <= 120)
    {
        wdtValue = WDTO_120MS;
    }
    else if (timeoutMs <= 250)
    {
        wdtValue = WDTO_250MS;
    }
    else if (timeoutMs <= 500)
    {
        wdtValue = WDTO_500MS;
    }
    else if (timeoutMs <= 1000)
    {
        wdtValue = WDTO_1S;
    }
    else if (timeoutMs <= 2000)
    {
        wdtValue = WDTO_2S;
    }
    else if (timeoutMs <= 4000)
    {
        wdtValue = WDTO_4S;
    }
    else
    {
        wdtValue = WDTO_8S;
    }
    wdt_enable(wdtValue);
}

void hal::watchdogDisable()
{
    wdt_disable();
}

void hal::watchdogReset()
{
    wdt_reset();
}

size_t hal::getFreeHeap()
{
    int v;
    return static_cast<size_t>(
        reinterpret_cast<int>(&v) -
        (__brkval == 0 ? reinterpret_cast<int>(&__heap_start) : reinterpret_cast<int>(__brkval))
    );
}

size_t hal::getTotalRam()
{
    // ATmega2560 has 8 KB SRAM
    return 8192;
}

#endif // PLATFORM_AVR
