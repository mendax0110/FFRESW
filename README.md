# FFRESW

## Overview

FFRESW is the embedded software for the Farnsworth Fusion Reactor project.  
It is designed for the Arduino MEGA2560 platform and provides modular control, data acquisition, and communication for the reactor system.

---

## Repository Structure

```
FFRESW/
├── .github/           # GitHub workflows and CI
├── docs/              # Documentation (Doxygen, PlantUML, etc.)
├── eSW/               # Embedded software source code
│   ├── FFRESW/        # Main firmware and project files
│   ├── libraries/     # Core and custom Arduino libraries
│   └── utils/         # Utility scripts and tools
├── external/          # External dependencies (submodules)
├── testing/           # Python and shell scripts for testing
├── LICENSE
├── README.md
```

---

## Getting Started

### Prerequisites

- **Arduino IDE**: Version 1.8.13 recommended
- **Sloeber IDE**: For advanced development ([Download](https://eclipse.baeyens.it/stable.php?OS=Windows))
- **Python 3**: For running test scripts in `testing/`
- **Git**: For cloning and managing submodules

### Build Steps

1. **Clone the repository**
    ```sh
    git clone https://github.com/mendax0110/FFRESW.git
    cd FFRESW
    ```

2. **Initialize submodules**
    ```sh
    git submodule update --init --recursive
    ```

3. **Prepare Memory Usage Library**
    - On Windows, run:
      ```sh
      cd utils
      copy_ErriezMemoryUsage.bat
      ```
    - On Linux, ensure the `ErriezMemoryUsage` library is available in `external/`.

4. **Install Required Arduino Libraries**
    - In Arduino IDE:  
      `Sketch > Include Library > Manage Libraries`
    - Install:
      - FreeRTOS
      - frt
      - ArduinoJson

5. **IDE and Compiler Settings**
    - See `docs/pictures/image-1.png` and `docs/pictures/image.png` for recommended settings.

6. **Error: MemoryFree**
    - If you encounter a "MemoryFree" error, ensure the `ErriezMemoryUsage` library is correctly added to your Arduino libraries.

---

## Development

- **Main Firmware**:  
  Located in [`eSW/FFRESW/`](eSW/FFRESW/)
- **Libraries**:  
  Custom and third-party libraries in [`eSW/libraries/`](eSW/libraries/)
- **Documentation**:  
  - [Doxygen-generated HTML](docs/doxygen/html/index.html)
  - [PlantUML diagrams](docs/PlantUML/)
  - [Overview and guides](docs/overview.md)
- **Testing**:  
  Python and shell scripts in [`testing/`](testing/)

---

## Documentation

- **Doxygen**:  
  Run Doxygen in `docs/doxygen/` to generate code documentation.
- **Architecture Diagrams**:  
  See [`docs/PlantUML/Library_Architecture.txt`](docs/PlantUML/Library_Architecture.txt) and [`docs/PlantUML/Github_Structure.txt`](docs/PlantUML/Github_Structure.txt).

---

## License

This project is licensed under the terms of the [LICENSE](LICENSE) file.

