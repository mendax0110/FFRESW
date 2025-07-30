 # FFRESW Project Overview

## Purpose

FFRESW is the embedded software platform for the Farnsworth Fusion Reactor project.  
It is designed to run on the Arduino MEGA2560, providing modular control, data acquisition, and communication for the reactor system.

---

## Key Features

- **Modular Architecture:**  
  Each subsystem (sensors, control, communication, etc.) is implemented as a separate module for maintainability and scalability.

- **Real-Time Operation:**  
  Utilizes FreeRTOS and related libraries for multitasking and real-time scheduling.

- **Data Acquisition & Logging:**  
  Collects, processes, and logs data from various sensors and subsystems.

- **Extensible Libraries:**  
  Custom and third-party libraries are organized for easy integration and reuse.

- **Testing & Utilities:**  
  Includes Python and shell scripts for automated testing and utility tasks.

---

## Directory Structure

- **eSW/FFRESW/**  
  Main firmware source code and project files.

- **eSW/libraries/**  
  Core and custom Arduino libraries, organized by module (e.g., `calcModule`, `comModule`, `logManager`, etc.).

- **eSW/utils/**  
  Utility scripts for build and analysis (e.g., memory usage, ELF analysis).

- **external/**  
  External dependencies and submodules.

- **docs/**  
  Documentation, including Doxygen and PlantUML diagrams.

- **testing/**  
  Scripts for automated testing and validation.

---

## Main Modules

- **calcModule:** Calculation utilities and algorithms.
- **comModule:** Communication interfaces and protocols.
- **flyback:** High Voltage Module interface and statemachine.
- **lockGuard:** Mutex Scoped Locking System.
- **jsonModule:** Json Library for communication with HAS and other devices.
- **pressure, temperature, vacControl:** Specialized modules for reactor environment control.
- **logManager:** Logging and data management.
- **sensorModule:** Sensor data acquisition and processing.
- **taskWatchDog:** Task monitoring and watchdog functionality.
- **serialMenu:** Serial interface for user interaction.
- **ptrUtils:** Pointer and memory management library.
- **timeModule:** Timemanagement library.
- **reportSystem:** System to observe state of uC as well as interfaces, memory and many other things.

---

## Development Workflow

1. **Clone and initialize submodules:**  
   See [README.md](../README.md#getting-started) for setup instructions.

2. **Build and upload firmware:**  
   Use Arduino IDE or Sloeber IDE with recommended settings.

3. **Run and monitor:**  
   Use the serial menu and logging features for system interaction and debugging.

4. **Test and validate:**  
   Use scripts in `testing/` for automated checks.

---

## Documentation

- **Doxygen:**  
  Generate code documentation from `docs/doxygen/`.

- **Architecture Diagrams:**  
  See PlantUML diagrams in `docs/PlantUML/`.

---

## License

This project is licensed under the terms of the [LICENSE](LICENSE) file.