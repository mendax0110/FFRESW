/**
 * @file hal_mcxn947.cpp
 * @author Adrian Goessl
 * @brief HAL implementation for NXP FRDM-MCXN947 developer board.
 * @version 1.0
 * @date 2026-03-07
 *
 * @copyright Copyright (c) 2026
 *
 * @details Maps the abstract HAL interface to the MCUXpresso SDK / FRDM-MCXN947
 *          board support package.  This file is only compiled when
 *          PLATFORM_MCXN947 is defined.
 *
 * @note Requires MCUXpresso SDK with board support for FRDM-MCXN947.
 *       This is a skeleton implementation; the SDK-specific calls should be
 *       filled in once the BSP is integrated.
 */

#include "hal.h"

#if defined(PLATFORM_MCXN947)

#include "fsl_gpio.h"
#include "fsl_lpadc.h"
#include "fsl_lpuart.h"
#include "fsl_lpi2c.h"
#include "fsl_lpspi.h"
#include "fsl_ctimer.h"
#include "fsl_wwdt.h"
#include "fsl_flash.h"
#include "fsl_common.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "board.h"

/* ------------------------------------------------------------------ */
/*  Internal helpers                                                   */
/* ------------------------------------------------------------------ */

static volatile uint32_t s_millisCounter = 0;

extern "C" void SysTick_Handler(void)
{
    s_millisCounter++;
}

/* ------------------------------------------------------------------ */
/*  GPIO                                                               */
/* ------------------------------------------------------------------ */

void hal::gpioSetMode(pin_t pin, PinMode mode)
{
    uint8_t port = static_cast<uint8_t>(pin >> 8);
    uint8_t bit  = static_cast<uint8_t>(pin & 0xFF);

    gpio_pin_config_t config;
    switch (mode)
    {
        case PinMode::Input:
            config.pinDirection = kGPIO_DigitalInput;
            break;
        case PinMode::Output:
            config.pinDirection = kGPIO_DigitalOutput;
            config.outputLogic  = 0;
            break;
        case PinMode::InputPullup:
            config.pinDirection = kGPIO_DigitalInput;
            break;
    }

    GPIO_Type* base = (port == 0) ? GPIO0 : (port == 1) ? GPIO1 : GPIO2;
    GPIO_PinInit(base, bit, &config);
}

void hal::gpioWrite(pin_t pin, PinState state)
{
    uint8_t port = static_cast<uint8_t>(pin >> 8);
    uint8_t bit  = static_cast<uint8_t>(pin & 0xFF);
    GPIO_Type* base = (port == 0) ? GPIO0 : (port == 1) ? GPIO1 : GPIO2;
    GPIO_PinWrite(base, bit, state == PinState::High ? 1U : 0U);
}

hal::PinState hal::gpioRead(pin_t pin)
{
    uint8_t port = static_cast<uint8_t>(pin >> 8);
    uint8_t bit  = static_cast<uint8_t>(pin & 0xFF);
    GPIO_Type* base = (port == 0) ? GPIO0 : (port == 1) ? GPIO1 : GPIO2;
    return GPIO_PinRead(base, bit) ? PinState::High : PinState::Low;
}

/* ------------------------------------------------------------------ */
/*  ADC                                                                */
/* ------------------------------------------------------------------ */

uint16_t hal::adcRead(pin_t channel)
{
    lpadc_conv_result_t result;
    lpadc_conv_command_config_t cmdConfig;
    lpadc_conv_trigger_config_t trigConfig;

    LPADC_GetDefaultConvCommandConfig(&cmdConfig);
    cmdConfig.channelNumber = static_cast<uint32_t>(channel);
    LPADC_SetConvCommandConfig(ADC0, 1U, &cmdConfig);

    LPADC_GetDefaultConvTriggerConfig(&trigConfig);
    trigConfig.targetCommandId = 1U;
    LPADC_SetConvTriggerConfig(ADC0, 0U, &trigConfig);

    LPADC_DoSoftwareTrigger(ADC0, 1U);
    while (!LPADC_GetConvResult(ADC0, &result))
    {
    }
    return static_cast<uint16_t>(result.convValue);
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
    return s_millisCounter;
}

tick_us_t hal::microsTick()
{
    // SysTick is a 24-bit down-counter
    uint32_t ticks = SysTick->LOAD - SysTick->VAL;
    uint32_t usPerTick = 1000000UL / (HAL_CLOCK_HZ / SysTick->LOAD);
    return s_millisCounter * 1000UL + (ticks * usPerTick / SysTick->LOAD);
}

void hal::delayMs(tick_ms_t ms)
{
    tick_ms_t start = millisTick();
    while ((millisTick() - start) < ms)
    {
    }
}

void hal::delayUs(tick_us_t us)
{
    tick_us_t start = microsTick();
    while ((microsTick() - start) < us)
    {
    }
}

/* ------------------------------------------------------------------ */
/*  PWM / Timer (CTimer)                                               */
/* ------------------------------------------------------------------ */

void hal::pwmInit(uint8_t channel)
{
    ctimer_config_t config;
    CTIMER_GetDefaultConfig(&config);
    CTIMER_Init(CTIMER0, &config);
    (void)channel;
}

void hal::pwmSetFrequencyAndDuty(uint8_t channel, uint32_t frequencyHz, uint8_t dutyPercent)
{
    if (frequencyHz == 0)
    {
        return;
    }
    CTIMER_SetupPwm(CTIMER0,
                     static_cast<ctimer_match_t>(channel),
                     dutyPercent,
                     frequencyHz,
                     CLOCK_GetCTimerClkFreq(0U),
                     false);
    CTIMER_StartTimer(CTIMER0);
}

void hal::pwmStop(uint8_t channel)
{
    (void)channel;
    CTIMER_StopTimer(CTIMER0);
}

/* ------------------------------------------------------------------ */
/*  UART / Serial (LPUART)                                             */
/* ------------------------------------------------------------------ */

static LPUART_Type* getLpuartBase(uint8_t port)
{
    switch (port)
    {
        case 1:  return LPUART1;
        case 2:  return LPUART2;
        default: return LPUART0;
    }
}

void hal::uartInit(uint8_t port, uint32_t baudRate)
{
    lpuart_config_t config;
    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps = baudRate;
    config.enableTx = true;
    config.enableRx = true;
    LPUART_Init(getLpuartBase(port), &config, CLOCK_GetLPFlexCommClkFreq(port));
}

void hal::uartWrite(uint8_t port, const uint8_t* data, size_t length)
{
    LPUART_WriteBlocking(getLpuartBase(port), data, length);
}

void hal::uartPrint(uint8_t port, const char* str)
{
    LPUART_WriteBlocking(getLpuartBase(port),
                         reinterpret_cast<const uint8_t*>(str),
                         strlen(str));
}

void hal::uartPrintln(uint8_t port, const char* str)
{
    uartPrint(port, str);
    const uint8_t crlf[] = {'\r', '\n'};
    LPUART_WriteBlocking(getLpuartBase(port), crlf, 2);
}

size_t hal::uartAvailable(uint8_t port)
{
    uint32_t flags = LPUART_GetStatusFlags(getLpuartBase(port));
    return (flags & kLPUART_RxDataRegFullFlag) ? 1 : 0;
}

int hal::uartRead(uint8_t port)
{
    if (uartAvailable(port) == 0)
    {
        return -1;
    }
    uint8_t byte;
    LPUART_ReadBlocking(getLpuartBase(port), &byte, 1);
    return static_cast<int>(byte);
}

bool hal::uartReady(uint8_t port)
{
    (void)port;
    return true;
}

void hal::uartEnd(uint8_t port)
{
    LPUART_Deinit(getLpuartBase(port));
}

/* ------------------------------------------------------------------ */
/*  I2C (LPI2C)                                                        */
/* ------------------------------------------------------------------ */

void hal::i2cInit()
{
    lpi2c_master_config_t config;
    LPI2C_MasterGetDefaultConfig(&config);
    config.baudRate_Hz = 100000U;
    LPI2C_MasterInit(LPI2C0, &config, CLOCK_GetLPI2cClkFreq());
}

bool hal::i2cWrite(uint8_t address, const uint8_t* data, size_t length)
{
    lpi2c_master_transfer_t xfer;
    memset(&xfer, 0, sizeof(xfer));
    xfer.slaveAddress = address;
    xfer.direction    = kLPI2C_Write;
    xfer.data         = const_cast<uint8_t*>(data);
    xfer.dataSize     = length;
    xfer.flags        = kLPI2C_TransferDefaultFlag;
    return LPI2C_MasterTransferBlocking(LPI2C0, &xfer) == kStatus_Success;
}

bool hal::i2cRead(uint8_t address, uint8_t* buffer, size_t length)
{
    lpi2c_master_transfer_t xfer;
    memset(&xfer, 0, sizeof(xfer));
    xfer.slaveAddress = address;
    xfer.direction    = kLPI2C_Read;
    xfer.data         = buffer;
    xfer.dataSize     = length;
    xfer.flags        = kLPI2C_TransferDefaultFlag;
    return LPI2C_MasterTransferBlocking(LPI2C0, &xfer) == kStatus_Success;
}

void hal::i2cEnd()
{
    LPI2C_MasterDeinit(LPI2C0);
}

/* ------------------------------------------------------------------ */
/*  SPI (LPSPI)                                                        */
/* ------------------------------------------------------------------ */

void hal::spiInit()
{
    lpspi_master_config_t config;
    LPSPI_MasterGetDefaultConfig(&config);
    config.baudRate = 1000000U;
    LPSPI_MasterInit(LPSPI0, &config, CLOCK_GetLPSpiClkFreq());
}

uint8_t hal::spiTransfer(uint8_t data)
{
    lpspi_transfer_t xfer;
    uint8_t rx;
    xfer.txData   = &data;
    xfer.rxData   = &rx;
    xfer.dataSize = 1;
    xfer.configFlags = kLPSPI_MasterPcsContinuous;
    LPSPI_MasterTransferBlocking(LPSPI0, &xfer);
    return rx;
}

void hal::spiEnd()
{
    LPSPI_Deinit(LPSPI0);
}

/* ------------------------------------------------------------------ */
/*  Non-volatile storage (Flash emulated EEPROM)                       */
/* ------------------------------------------------------------------ */

/// @brief Flash-based NV storage base address (last 4 KB sector of flash)
static const uint32_t NV_FLASH_BASE = 0x000FF000UL;

bool hal::nvWrite(uint16_t address, const void* data, size_t length)
{
    flash_config_t flashConfig;
    FLASH_Init(&flashConfig);

    status_t status = FLASH_Program(
        &flashConfig,
        NV_FLASH_BASE + address,
        const_cast<uint8_t*>(static_cast<const uint8_t*>(data)),
        length
    );
    return status == kStatus_Success;
}

bool hal::nvRead(uint16_t address, void* data, size_t length)
{
    memcpy(data, reinterpret_cast<const void*>(NV_FLASH_BASE + address), length);
    return true;
}

/* ------------------------------------------------------------------ */
/*  System                                                             */
/* ------------------------------------------------------------------ */

void hal::systemReset()
{
    NVIC_SystemReset();
}

void hal::watchdogEnable(uint16_t timeoutMs)
{
    wwdt_config_t config;
    WWDT_GetDefaultConfig(&config);
    config.timeoutValue = (timeoutMs * (CLOCK_GetWdtClkFreq() / 1000)) / 4;
    config.enableWatchdog = true;
    WWDT_Init(WWDT, &config);
}

void hal::watchdogDisable()
{
    WWDT_Disable(WWDT);
}

void hal::watchdogReset()
{
    WWDT_Refresh(WWDT);
}

size_t hal::getFreeHeap()
{
    extern uint32_t __HeapLimit;
    extern uint32_t __StackLimit;
    volatile uint8_t stackVar;
    return reinterpret_cast<size_t>(&stackVar) - reinterpret_cast<size_t>(&__HeapLimit);
}

size_t hal::getTotalRam()
{
    // MCXN947 has 512 KB SRAM
    return 512 * 1024;
}

#endif // PLATFORM_MCXN947
