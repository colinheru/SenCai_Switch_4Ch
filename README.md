# KNX Iot 

This projects provides a knx-device stack for arduino (ESP8266, ESP32, SAMD21, RP2040, STM32), CC1310 and linux. (more are quite easy to add)
It implements most of System-B specification and can be configured with ETS.
优点：
1.ETS编程配置；
2.KNX_TP方式组网；
3.不受KNX_TP芯片库存限制；
4.成本低，无需KNX_TP IC依赖和KNX总线安装；
5.安装方便，无需KNX系统电源和KNX总线；
6. 与现有的KNX系统兼容（且易于升级/扩展）；


## Stack configuration possibilities

Specify prog button GPIO other then `GPIO0`:
```C++
knx.buttonPin(3); // Use GPIO3 Pin
```

Specify a LED GPIO for programming mode other then the `LED_BUILTIN`:
```C++
knx.ledPin(5);
```

Use a custom function instead of a LED connected to GPIO to indicate the programming mode:
```C++
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <knx.h>
// create a pixel strand with 1 pixel on PIN_NEOPIXEL
Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL);

void progLedOff()
{
  pixels.clear();
  pixels.show();
}

void progLedOn()
{
  pixels.setPixelColor(0, pixels.Color(20, 0, 0));
  pixels.show();
}

void main ()
{
 knx.setProgLedOffCallback(progLedOff);
 knx.setProgLedOnCallback(progLedOn);
 [...]
}
```


