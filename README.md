# Standalone Tank IR 

The files in the TankIR folder consist of an Arduino sketch that can be loaded onto any Arduino Duemilanove/Uno/Nano or other board with an ATmega328 processor. 

With the addition of an IR receiver and emitter this device can send and receive any RC tank protocol. IR will be sent whenever pin D4 is connected to ground, for example with a pushbutton. 

Several configuration settings are available in the "A_Setup.h" file (it will appear as a tab in your sketch). This includes which protocols are sent and which are received. You can also set the device to send repair signals, and you can even have the device respond by sending a repair signal whenever it receives a "hit" - this could be used for a static tank "repair station." 

You can attach a servo to pin D8 and a high intensity flash to D6 and they will also activate whenever the cannon is fired. Please see the schematic PDF for more details. 

Leave your Arduino attached to your computer with a USB cable and open the Serial Monitor from within the Arduino IDE to see informational messages printed during operation. 


# Hardware Notes

You can connect a standard Tamiya "apple" directly to your Arduino - [Tamiya 53447](https://www.tamiyausa.com/product/item.php?product-id=53447). The Tamiya apple includes notification LEDs that already have current limiting resistors included. If you build your own receiver and use your own hit notification LEDs, you need to include your own current limiting resistor appropriate to the LEDs you choose. Most Arduinos can't source more than 40mA per pin. 

For the IR transmitter you can use the Tamiya IR LED that is included with the apple. We have also found the Vishay TSAL6100 ([DigiKey 751-1203-ND](http://www.digikey.com/product-detail/en/TSAL6100/751-1203-ND/1681338)) to be a comparable replacement. 

For maximum distance the IR transmitter should be driven far beyond its typical rating. The LED will survive because the IR signal is very brief. In testing we have found a 3.3 ohm, 1 watt resistor to be the best compromise between distance and LED longevity. 

If you wish to send repair signals it is often desired to prevent the beam from traveling very far. In this case a higher value resistor is used - we have found 1k ohm will give you a range of just a few feet. 

## License
All Open Panzer firmware is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

For more specific details see [http://www.gnu.org/licenses](http://www.gnu.org/licenses), the [Quick Guide to GPLv3.](http://www.gnu.org/licenses/quick-guide-gplv3.html) and the [copying.txt](https://github.com/OpenPanzerProject/TCB/blob/master/COPYING.txt) file in the codebase.

The GNU operating system which is under the same license has an informative [FAQ here](http://www.gnu.org/licenses/gpl-faq.html).
