/**
 * @file platform.h
 * @author Adrian Goessl
 * @brief Platform detection and configuration for multi-target builds.
 * @version 1.0
 * @date 2026-03-07
 *
 * @copyright Copyright (c) 2026
 *
 * @details Detects the target platform at compile time and defines platform
 *          macros used throughout the project. Supported targets:
 *          - PLATFORM_AVR: Arduino ATmega2560
 *          - PLATFORM_MCXN947: NXP FRDM-MCXN947
 */
#ifndef PLATFORM_H
#define PLATFORM_H

/* ------------------------------------------------------------------ */
/*  Platform detection                                                 */
/* ------------------------------------------------------------------ */

#if defined(__AVR_ATmega2560__) || defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO)
    #define PLATFORM_AVR 1
    #define PLATFORM_NAME "AVR ATmega2560"
#elif defined(__MCXN947__) || defined(CPU_MCXN947VDF) || defined(FRDM_MCXN947)
    #define PLATFORM_MCXN947 1
    #define PLATFORM_NAME "NXP FRDM-MCXN947"
#else
    #error "Unsupported platform. Define PLATFORM_AVR or PLATFORM_MCXN947."
#endif

/* ------------------------------------------------------------------ */
/*  Common type aliases                                                */
/* ------------------------------------------------------------------ */

#include <stdint.h>
#include <stddef.h>

/// @brief Type alias for GPIO pin identifiers \typedef pin_t
typedef uint16_t pin_t;

/// @brief Type alias for millisecond tick counts \typedef tick_ms_t
typedef uint32_t tick_ms_t;

/// @brief Type alias for microsecond tick counts \typedef tick_us_t
typedef uint32_t tick_us_t;

/* ------------------------------------------------------------------ */
/*  Feature flags                                                      */
/* ------------------------------------------------------------------ */

#if defined(PLATFORM_AVR)
    #define HAL_HAS_EEPROM       1
    #define HAL_HAS_PROGMEM      1
    #define HAL_HAS_WATCHDOG     1
    #define HAL_CLOCK_HZ         16000000UL
    #define HAL_ADC_RESOLUTION   10
    #define HAL_ADC_MAX_VALUE    1023
    #define HAL_VCC              5.0f
#elif defined(PLATFORM_MCXN947)
    #define HAL_HAS_EEPROM       0
    #define HAL_HAS_PROGMEM      0
    #define HAL_HAS_WATCHDOG     1
    #define HAL_CLOCK_HZ         150000000UL
    #define HAL_ADC_RESOLUTION   16
    #define HAL_ADC_MAX_VALUE    65535
    #define HAL_VCC              3.3f
#endif

/* ------------------------------------------------------------------ */
/*  PROGMEM compatibility                                              */
/* ------------------------------------------------------------------ */

#if HAL_HAS_PROGMEM
    // AVR keeps the Arduino PROGMEM/F() macros from <avr/pgmspace.h>
#else
    // On ARM the flash is memory-mapped; PROGMEM is a no-op
    #ifndef PROGMEM
        #define PROGMEM
    #endif
    #ifndef F
        #define F(string_literal) (string_literal)
    #endif
    #ifndef pgm_read_byte
        #define pgm_read_byte(addr)  (*(const uint8_t  *)(addr))
    #endif
    #ifndef pgm_read_word
        #define pgm_read_word(addr)  (*(const uint16_t *)(addr))
    #endif
    #ifndef pgm_read_dword
        #define pgm_read_dword(addr) (*(const uint32_t *)(addr))
    #endif
#endif

#endif // PLATFORM_H
