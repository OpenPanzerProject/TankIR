# Standalone Tank IR 

The files in the TankIR folder consist of an Arduino sketch that can be loaded onto any Arduino Duemilanove/Uno/Nano or other board with an ATmega328 processor. 

With the addition of an IR receiver and emitter this device can send and receive any RC tank protocol. IR will be sent whenever pin D4 is connected to ground, for example with a pushbutton. 

Several configuration settings are available in the "A_Setup.h" file (it will appear as a tab in your sketch). This includes which protocols are sent and which are received. You can also set the device to send repair signals, and you can even have the device respond by sending a repair signal whenever it receives a "hit" - this could be used for a static tank "repair station." 

You can attach a servo to pin D8 and a high intensity flash to D6 and they will also activate whenever the cannon is fired. Please see the schematic PDF for more details. 

Leave your Arduino attached to your computer with a USB cable and open the Serial Monitor from within the Arduino IDE to see informational messages printed during operation. 

## License
All Open Panzer firmware is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

For more specific details see [http://www.gnu.org/licenses](http://www.gnu.org/licenses), the [Quick Guide to GPLv3.](http://www.gnu.org/licenses/quick-guide-gplv3.html) and the [copying.txt](https://github.com/OpenPanzerProject/TCB/blob/master/COPYING.txt) file in the codebase.

The GNU operating system which is under the same license has an informative [FAQ here](http://www.gnu.org/licenses/gpl-faq.html).
