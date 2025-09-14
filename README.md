# ESP32_TOTP 🔐

A compact and secure TOTP (Time-Based One-Time Password) generator built on the ESP32 platform. Designed for two-factor authentication (2FA) systems like Google Authenticator, this project displays time-based codes on a small OLED screen and syncs with an RTC module for accurate timekeeping.

## 📸 Preview

<img src="/IMAGES/1.JPG" alt="drawing" width="400"/>
<img src="/IMAGES/2.JPG" alt="drawing" width="400"/>
<img src="/IMAGES/3.JPG" alt="drawing" width="400"/>

You can find more images in `IMAGES` folder

## 🚀 Features

- Generates 6-digit TOTP codes (RFC 6238 compliant)
- Assembly and flashing process takes about 15 minutes
- Battery Life is 2-3 Months (depends on a battery) 
- OLED display (SSD1306 128x64 I2C)
- Battery-powered with charging module (IP5310)
- Real-Time Clock (DS3231) for accurate timekeeping
- Compact hardware footprint (ESP32-C3 SuperMini)

## 🧰 Software Features

- Configurable AP wifi and password 
- Configuration in WEB BASED interface (Captive portal)
- Secrets reset on wrong pincode entry (after 3 tries) 

## 🛠️ Hardware (BOM)

| #  | Part Name                         | Quantity | Link                                                                                             |
|----|----------------------------------|----------|--------------------------------------------------------------------------------------------------|
| 1  | ESP32-C3 SuperMini               | 1 PCS    | [AliExpress](https://www.aliexpress.com/item/1005007205044247.html)                              |
| 2  | DS3231SN RTC                     | 1 PCS    | [AliExpress](https://www.aliexpress.com/item/1005008628337406.html)                              |
| 3  | SSD1306 OLED Display (128x64)    | 1 PCS    | [AliExpress](https://www.aliexpress.com/item/1005006141235306.html)                              |
| 4  | IP5310 Battery Charging Module   | 1 PCS    | [AliExpress](https://www.aliexpress.com/item/1005005632666389.html)                              |
| 5  | RT9080-33GJ5 Voltage Regulator   | 1 PCS    | [AliExpress](https://www.aliexpress.com/item/1005006832947988.html)                              |
| 6  | 10K SMD Resistor                 | 9 PCS    | [AliExpress](https://www.aliexpress.com/item/1005005600798857.html)                              |
| 7  | BAT54 KL1 Schottky Diode        | 1 PCS    | [AliExpress](https://www.aliexpress.com/item/1005007167113616.html)                              |
| 8  | 1µF SMD Capacitor                | 2 PCS    | [AliExpress](https://www.aliexpress.com/item/32965092877.html)                                   |
| 9  | SMD 3×4mm Tactile Button         | 4 PCS    | [AliExpress](https://www.aliexpress.com/item/4001107238344.html)                                 |
| 10 | MSK-12C02 Slide Switch           | 1 PCS    | [AliExpress](https://www.aliexpress.com/item/1005009322115934.html)                              |
| 11 | LI-ION battery from VAPE         | 1 PCS    |                               |


>[!NOTE]
> The battery size should be no more than `42mm`

<img src="/IMAGES/BATTERIES.JPG" alt="drawing" width="400"/>

## 🔧 Getting Started

1. **Order the PCB**

    <img src="/IMAGES/PCB.JPG" alt="drawing" width="400"/>

    [EASYEDA/JLCPCB](https://oshwlab.com/bundemshake/esp32_totp)

    Here you can look at the schematic, PCB, order the PCB
    You can also use a gerber file from `PCB/gerber.zip`

    When ordering the pcb all settings should be left as default except:

    - 2 Layers
    - 1.6mm PCB Thickness

2. **Print the case, you can find 3D models in `CAD` folder, use such settings:** 
    - Filament: Any, i've used PLA
    - Layer height: 0.08mm
    - Supports: None
    - Walls: Outer first

3. **Assemble the PCB and install it in the case**
    - You can use a soldering fan to solder components, i've used a soldering iron with a thin tip 
    - Be careful to not overheat the ESP32 because it can make it inoperable 

    Here is the assembled PCB in it's case:

    <img src="/IMAGES/STEP1.JPG" alt="drawing" width="400"/>
    <img src="/IMAGES/STEP2.JPG" alt="drawing" width="400"/>
    <img src="/IMAGES/STEP3.JPG" alt="drawing" width="400"/>

4. **Flash Firmware**
   - Use PlatformIO extension in VSCode 
   - Configure AP credentials in `include/config.h`
   
   **Flashing process**
    1. Connect the ESP

    2. Make sure your computer sees it, on Linux and MacOS you can check it by typing `ls /dev/tty*` in terminal,
    esp should be called `ttyACM0`

    3. In platformio go to `Project Tasks/Platform` 

    4. Press `Erase Flash`, wait for flash to be erased, if it doesn't work from first time, try again 

    5. Press `Build Filesystem Image`

    6. Press `Upload Filesystem Image`

    7. Upload the code


## 📚 Libraries Used

- esphome/ESPAsyncWebServer-esphome@^3.4.0
- bblanchon/ArduinoJson@^7.4.1
- arduino-libraries/NTPClient@^3.2.1
- adafruit/RTClib@^2.1.4
- https://github.com/dirkx/Arduino-Base32-Decode.git
- adafruit/Adafruit GFX Library@^1.12.1
- adafruit/Adafruit SSD1306@^2.5.14
- olikraus/U8g2_for_Adafruit_GFX@^1.8.0
- lucadentella/TOTP library@^1.1.0
- danilopinotti/Battery_18650_Stats@^1.0.0

---

> **Note**: This project is not intended to replace your mobile 2FA app but is a great educational and offline alternative for understanding TOTP generation.

