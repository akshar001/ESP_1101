# CC1101_ESP32/ESP8266

An Arduino library for ESP8266/ESP32 and CC1101

# Connecting an CC1101 to ESP8266
These instructions are for an Arduino Uno.

CLK - D5
MISO - D6
MOSI - D7
CS - D8
GDO0 PIN - GPIO5

# Connecting an CC1101 to ESP8266


SCK_PIN   14
MISO_PIN  12
MOSI_PIN  13
SS_PIN    15
GDO0 PIN  26


# Installing the Library

To install the library into your IDE:
* click on the Clone or Download button on this Github page and select Download ZIP.
* Start the Arduino IDE and from the Sketch menu do Sketch->Include Library->Add ZIP Library and select the ZIP you just downloaded.


#API Reference

This is a very easy library to use. You may just wish to try out the examples, that send a text message from one Arduino to another using the Serial Monitor. But for completeness, here it is:


## Include File

```
#include <ELECHOUSE_CC1101.h>
```


## Initialisation

Put this in your setup function.

```
ELECHOUSE_cc1101.Init(F_433); // set frequency - F_433, F_868, F_965 MHz
```


## Send/Receive Mode

Put this in your setup function and call again, any time after you have processed a received message.




```
 ELECHOUSE_cc1101.SetReceive();
```


## Receiving Data

The maximum data size is 64 bits.

ReceiveData requires a buffer of type byte[] and returns the number of bytes contained in the message.

```
int len = ELECHOUSE_cc1101.ReceiveData(buffer);
```



## Sending Data

The maximum data size is 64 bits.

SendData requires a buffer of type byte[] and the number of bytes contained in the message.

```
ELECHOUSE_cc1101.SendData(buffer, len);
```
