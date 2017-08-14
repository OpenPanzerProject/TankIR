# Standalone Tank IR 

The files in the TankIR folder consist of an Arduino sketch that can be loaded onto any Arduino Duemilanove/Uno/Nano or other board with an ATmega328 processor. 

With the addition of an IR receiver and emitter this sketch can send and receive any RC tank protocol. 

IR can be sent ("cannon fire") in two ways: 
  * Whenever pin D4 is connected to ground, for example with a pushbutton. 
  * Whenever positive 5 volts is detected on pin A0, for example a signal from another device.

You can attach a recoil servo to pin D8 and a Taigen High Intensity flash to D6 and they will also activate whenever the cannon is fired. 

Several configuration settings are available in the "A_Setup.h" file (it will appear as a tab in your sketch). Recoil servo end points and retract and return times can all be adjusted in this file, as well as which protocols are sent and which are received. You can also set the device to send repair signals, and you can even have the device respond by sending a repair signal whenever it receives a "hit" - this would be useful for a static tank "repair station." 

Leave your Arduino attached to your computer with a USB cable and open the Serial Monitor from within the Arduino IDE to see informational messages printed during operation. 


# Hardware Notes

Please see the schematic PDF for information on wiring. An Arduino Nano is shown but the pins are the same for the Uno. 

You can connect a standard Tamiya "apple" directly to your Arduino ([Tamiya 53447](https://www.tamiyausa.com/product/item.php?product-id=53447)). The Tamiya apple combines an IR receiver and notification LEDs; the LEDs already have current limiting resistors included inside the apple. If you build your own receiver and use your own hit notification LEDs, you need to include your own current limiting resistor appropriate to the LEDs you choose. Most Arduinos can't source more than 40mA per pin. 

For the IR transmitter you can use the Tamiya IR LED that is included with the apple. We have also found the Vishay TSAL6100 ([DigiKey 751-1203-ND](http://www.digikey.com/product-detail/en/TSAL6100/751-1203-ND/1681338)) to be a comparable replacement. 

For maximum distance the IR transmitter should be driven far beyond its typical current rating, but even so it still needs a current limiting resistor. The LED will survive the high current because the IR signal is very brief. In testing we have found a 3.3 ohm, 1 watt resistor to be the best compromise between range and LED longevity. 

If you wish to send repair signals it is often desired to prevent the beam from traveling very far. In this case a higher value resistor is used inline with the IR emitter - we have found 1k ohm will give you a range of just a few feet. 

## Interfacing With Adafruit Audio FX Boards
You can add sound to your project by interfacting with an Adafruit Audio FX board. Adafruit sells several versions of this product with different amounts of on-board memory and different outputs. Unless you are using an external amplifier, you probably want the ones with a built-in amp, such as this [16 MB version](https://www.adafruit.com/product/2217) or this [2 MB version](https://www.adafruit.com/product/2210), both with built-in 2W amplifiers. 

Add the following sounds with the names shown to the FX board (plug it into your computer and just drag and drop the files): 
  * T01.wav - your cannon fire sound
  * T02.wav - your cannon "hit" sound (sound to play when device receives an IR hit)
  * T03.wav - your destroyed sound (device has received enough hits to be destroyed)
  * T04.wav - repair sound (sound that will play if being repaired, or if a repair station, a repair signal being sent out)

Now you must connect your Arduino to the Audio FX board

| Arduino Pin   | Audio FX Pin  |
| ------------- |---------------|
| A1      | 1 |
| A2      | 2 |
| A3      | 3 |
| A4      | 4 |
| GND     | GND |

Of course you will also need to provide power to your Audio FX board. See the [Adafruit Tutorial](https://learn.adafruit.com/adafruit-audio-fx-sound-board/overview) for more information on their device. 


# Example project
See this thread over at RC Tank Warfare where this project is interfaced with a standard Heng Long board to add Tamiya IR compatibility: [Arduino UNO IR Battle System](https://www.rctankwarfare.co.uk/forums/viewtopic.php?f=81&t=21941).

## License
All Open Panzer firmware is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

For more specific details see [http://www.gnu.org/licenses](http://www.gnu.org/licenses), the [Quick Guide to GPLv3.](http://www.gnu.org/licenses/quick-guide-gplv3.html) and the [copying.txt](https://github.com/OpenPanzerProject/TCB/blob/master/COPYING.txt) file in the codebase.

The GNU operating system which is under the same license has an informative [FAQ here](http://www.gnu.org/licenses/gpl-faq.html).
