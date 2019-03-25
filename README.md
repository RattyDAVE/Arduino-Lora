# Arduino-Lora
Heltec lora 32 - testing.

LoRachip support frequency band：  SX1276（868-915、923MHz）；SX1278(433-510MHz)



Libs

http://www.airspayce.com/mikem/arduino/RadioHead/

http://www.airspayce.com/mikem/arduino/RadioHead/RadioHead-1.89.zip

RH_RF95 Works with Semtech SX1276/77/78/79, Modtronix inAir4 and inAir9, and HopeRF RFM95/96/97/98 and other similar LoRa capable radios. Supports Long Range (LoRa) with spread spectrum frequency hopping, large payloads etc. FSK/GFSK/OOK modes are not (yet) supported.

```
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`

#include <SPI.h>
#include <RH_RF95.h>

SSD1306 display(0x3c, 4, 15);


// WIFI_LoRa_32 ports

// GPIO5 — SX1278’s SCK
// GPIO19 — SX1278’s MISO
// GPIO27 — SX1278’s MOSI
// GPIO18 — SX1278’s CS
// GPIO14 — SX1278’s RESET
// GPIO26 — SX1278’s IRQ(Interrupt Request)

#define SS 18
#define RST 14
#define DI0 26

// Singleton instance of the radio driver
RH_RF95 rf95(SS, DI0); // [heltec|ttgo] ESP32 Lora OLED with sx1278
```


