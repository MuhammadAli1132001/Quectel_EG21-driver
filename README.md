# Quectel_EG21-driver
Anyone can use this driver for cellular &amp; LTE communication for IoT based project
# Quectel EG21 Communication Library for STM32

## Overview
This library provides functions for interfacing an STM32 microcontroller with the Quectel EG21 modem via AT commands over UART. It includes functionalities for checking network registration, configuring MQTT, managing GPS, and controlling power modes.

## Features
- Send AT commands and receive responses
- Check modem connectivity
- Manage MQTT connections (publish/subscribe)
- Configure GPS
- Network registration status
- Power management for the modem

## Prerequisites
- **STM32 microcontroller** (tested with STM32G0 series)
- **Quectel EG21 modem**
- **UART communication enabled** (UART1 for modem, UART2 for debugging)
- **STM32 HAL library**

## Functions
### General AT Commands
```c
uint8_t checkAT(); // Check if the modem responds to AT commands
uint8_t Check_OkResponse(); // Verify if the response contains "OK"
void SIMTransmit(char *cmd, uint16_t timeout); // Send AT command and receive response
```

### IMEI & Signal Strength
```c
uint8_t check_IM(); // Check IMEI
uint8_t changeIMEI(char * NewIMEI); // Change IMEI
uint16_t checkRSSI(); // Get signal strength
```

### MQTT Functions
```c
uint8_t configMQTT(); // Configure MQTT keep-alive time
uint8_t OpenMQTTclient(char *Host, int Port); // Connect to an MQTT broker
uint8_t ConnectMQTTServer(char *cid, char *Username, char *Password); // Authenticate MQTT connection
uint8_t checkMQTTserver(); // Verify MQTT server connection
uint8_t publishtopicMQTT(char *Topic, char *data); // Publish a message to an MQTT topic
uint8_t subtopic(char *topic, uint8_t QoS); // Subscribe to an MQTT topic
uint8_t MQTTdiconnectserver(); // Disconnect from the MQTT server
```

### GPS Functions
```c
uint8_t GPSHOTstart(); // Start GPS
uint8_t GPSstop(); // Stop GPS
char* GPSINFO(); // Retrieve GPS information
```

### Network & Power Management
```c
uint8_t Network_Reg(); // Check network registration status
uint8_t Resetmodem(); // Reset the Quectel modem
uint8_t SetFULLFun(); // Set modem to full functionality
uint8_t SetMinFun(); // Set modem to minimum functionality
uint8_t Quectel_Power_down(); // Power down the modem
uint8_t Sleep_Quectel(); // Put the modem into sleep mode
uint8_t Config_ApReady(); // Configure AP_READY
uint8_t Config_URC(); // Configure URC settings
```

## Usage
1. **Initialize UART1 and UART2** in STM32CubeIDE or your preferred IDE.
2. Use `checkAT();` to verify modem communication.
3. Configure MQTT or GPS as needed.
4. Use `SIMTransmit();` to send custom AT commands.

## Notes
- Ensure **UART1 and UART2** are correctly initialized.
- Modify **buffer sizes** if needed for longer responses.
- Customize **MQTT topics and credentials** according to your broker settings.

## License
This project is **open-source** and free to use.

