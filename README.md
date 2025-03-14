## PlatformIO ESP32-based Energy Meter with Data Storage

## Overview
This repository contains a PlatformIO-based project designed for the ESP32-based Energy Meter with Data Storage. Follow the instructions below to set up PlatformIO, build the project, and upload it to your ESP32 development board.

## Prerequisites
Before proceeding, ensure you have the following:
- A compatible ESP32 development board
- USB cable for connection
- [VS Code](https://code.visualstudio.com/) or [PlatformIO IDE](https://platformio.org/install/ide)
- PlatformIO extension installed in VS Code
- Drivers or Library for your ESP32 (if needed)

## Installation and Setup
### 1. Clone the Repository
```bash
git clone https://github.com/paulcastor30/Energy-Meter-with-Data-Storage.git
cd your-repository-name
```

### 2. Install PlatformIO
If you haven't installed PlatformIO, follow these steps:
- Install [VS Code](https://code.visualstudio.com/)
- Install the [PlatformIO extension](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)
- Restart VS Code after installation

Alternatively, you can install PlatformIO CLI:
```bash
pip install platformio
```

### 3. Open the Project in VS Code
1. Open VS Code
2. Click **File** > **Open Folder...**
3. Select the cloned project folder
4. PlatformIO should automatically detect and configure the project

## Building and Uploading
### 1. Connect the ESP32 Board
- Plug in your ESP32 board via USB
- Check the correct COM port:
  - **Windows:** Open `Device Manager` > `Ports (COM & LPT)`
  - **Mac/Linux:** Run `ls /dev/tty.*` or `ls /dev/serial/by-id/`

### 2. Build the Project
Click on **Build** (checkmark icon in PlatformIO) or use the command:
```bash
pio run
```

### 3. Upload to ESP32
Click **Upload** (arrow icon in PlatformIO) or use the command:
```bash
pio run --target upload
```

### 4. Monitor Serial Output (Optional)
To check logs and debug messages, open the serial monitor:
```bash
pio device monitor
```

## Troubleshooting
### **1. Unable to detect ESP32 port?**
- Ensure your USB cable supports data transfer (not just charging)
- Try a different USB port or cable
- Install missing drivers (e.g., CP210x or CH340 drivers)

### **2. Upload fails or times out?**
- Press and hold the **BOOT** button on your ESP32 while uploading
- Check that PlatformIO is configured for the correct board in `platformio.ini`

### **3. Serial monitor shows garbage data?**
- Ensure the correct baud rate is set in `platformio.ini` (e.g., `monitor_speed = 115200`)


