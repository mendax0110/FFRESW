# FFRESW

## Overview

This project utilizes the [Arduino-MemoryFree](https://github.com/mpflaga/Arduino-MemoryFree) library to monitor memory usage in Arduino sketches. To get started, please follow the instructions below.

## Prerequisites

- You need to have the [Programino IDE](https://www.programino.com/) installed on your machine.

## Getting Started

### Build Steps

**1.** Clone the repository:
```bash
git clone https://github.com/mendax0110/FFRESW
cd FFRESW
```
**2.** Init submodule:
````bash
git submodule update --init --recursive
````

**3.** Run the copy_memory_free.bat file
```bash	
cd utils
copy_memory_free.bat
```

**4.** Arduino Version 1.8.13

**5.** Open the main.ino file in the Programino IDE.

**6.** IDE and Compiler Settings
![alt text](docs/pictures/image.png)

**7.** Install missing Files
(In the Arduino IDE) Sketch > Include Library > Manage Libraries.
(MegunoLink
ArduinoJson.h)



