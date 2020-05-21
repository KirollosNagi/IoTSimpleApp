# IoTSimpleApp

## Description
This is a repo providing a walk-through for a simple project to begin grasping the concepts of IoT. This project uses STM32L432KC as the microcontroller communicating with ESP8266 Wi-Fi Module and RTC DS3231 
This project was implemented for the course CSCE4301 - Section 1 Embedded Systems - AUC - Egypt

### Author

Kirollos N. Sorour ECNG Graduting Senior

### Project Prompt

Objective: To develop a time-aware, embedded smart solution to keep some loads (LEDs) connected to the Internet.

I will do so using an RTC and a Wi-Fi module which will take commands from an authorized online user and direct these commands to the microcontroller.

## Hardware Components

* Nucleo STM32L432KC/Blue Pill
* Adafruit Huzzah ESP8266 Wi-Fi Module
* RTC DS3231
* MicroUSB Cable
* TTL/USB Connector

## Prequisite Software

* Arduino v1.8.12
* ES8266 Library
* STM32CubeMX
* Keil uVision 5

## Building

There are two components to this project the arduino code to be flashed onto the ESP module and the keil code to be flashed onto the STM module.

Once you have installed the prequisite software proceed by compiling and flashing the code onto their respective modules.

## Running

To run the code just:
- Power up the modules
- Use your own SSID and Password
- Get the ESP device local ip from your gateway
- Use that ip to access the webserver

## Architecture

![alt text](https://github.com/KirollosNagi/IoTSimpleApp/blob/master/Support_Material/architecture.jpeg "Architecture IMG")

## Connections

### DISCLAIMER: 
IN THIS FRITIZING DRAWING, I AM USTING THE NUCLEO-64 ONLY BECAUSE NUCLEO-32 PART IS NOT AVAILABLE ONLINE. 

![alt text](https://github.com/KirollosNagi/IoTSimpleApp/blob/master/Support_Material/connections.png "Connections IMG")

### Actual pin Connection and configuration
* Make sure you have devices powered and sharing a common ground.

| STM32 | ESP8266 |
|---|---|
| UART1 TX (PB6/D5) | Tx pin |
| UART1 TX (PB7/D4)  | Rx pin |

| STM32 | DS3231 |
|---|---|
| I2C_SCL (PA9/D1) | I2C_SCL |
| I2C_SDA (PA10/D0) | I2C_SDA |

* Also Note the next pins should remain NC

| Pin | Functionality |
|---|---|
| STM32 UART2 Tx (PA2/A7) | (NC)(debug UART2 Tx) |
| STM32 UART2 Rx (PA15/Virtual) | (NC)(debug UART2 Rx) |
| STM32 GPIO LED (PB3/D12) | (NC)(Internal LED 3) |


## Functionality

* Retrieving the date/time from RTC module connected to the STM32 module
* Control the STM32 module LEDs status
* Control the WiFi module LEDs status
* Retrieving the temperature from RTC module
* Retrieving the time from the RTC module

## Communication Scheme Design 

### Commands from STM32 to ESP8266
| Command | Description |
|---|---|
| ! | Retrieve Local IP of ESP |
| P-"Temp" | Recieve and Display Temperature |
| E-"Time" | Recieve and Display Time |

### Commands from ESP8266 to STM32
| Command | Description |
|---|---|
| T_ON | Turn STM32 on-board LED ON |
| T_OF | Turn STM32 on-board LED OFF |
| TIME | Display Current Time from RTC |
| TEMP | Display Room Temperature from RTC |


The main idea driving this approach of designing commands was its simplicity since all the commands received at the STM32 side were of char size of 4 which made them easier to process using static containers. Moreover, an easier approach that I took was to design each command to have a different last character. Therefore, The STM32 board can easily analyze the commands based on the last character of each one of these commands.


## Future Considerations

* Adding an indicator screen
Given that this project will be utilized in smart homes as a product in the future, a good idea would be to add an LCD screen indicating the status of the system and individual components of it.
* Power calculation and choosing a sufficient battery
Since this project is going to be deployed heavily, we need to take into consideration the power consumption of the circuitry used in our system. Also, this system could be deployed with a fixed power source or using a battery. In the latter case, a good idea would be to examine the batteries in the market to provide the most reasonable lifetime for our system to be attractive for customers.
* Using advanced AT commands with the Wi-Fi module
I have spent some time searching many datasheets till I found the one for the Wi-Fi module we use, and it seems that this Wi-Fi module is very advanced and has many capabilities. One of these is that it can read and process AT commands which would be a great learning experience and also a great advancement for the controllability of the proposed system.
* Board PCB design and enclosure
Finally after all of the above is covered and the system is running and user friendly, I would propose to increase the system’s usability by introducing a factor of compactness into the game and going over the PCB design process and provide also an enclosure casing to it to make it look like an attractive on-the-shelf product.
* Implement FreeRTOS or similar functionalities in the microcontroller for better response time/extensibility
