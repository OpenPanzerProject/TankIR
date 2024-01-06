# Standalone Tank IR 

The files in the TankIR folder consist of an Arduino sketch that can be loaded onto any Arduino Duemilanove/Uno/Nano or other board with an ATmega328 processor. (In the Arduino IDE, go to the Tools menu and under the Board section select "Arduino Nano" or "Arduino AVR Boards -> Arduino Nano")

With the addition of an IR receiver and emitter this sketch can send and receive any RC tank protocol. 

# Hardware Notes 
Included in the project filesis a simple schematic in PDF format, please review it for wiring details. An Arduino Nano is shown but the pins are the same for the Uno.

## To trigger the cannon fire:
  * Manually - connect a pushbutton between Arduino pin D4 and ground.
  * From some other device - send a 5v input signal to Arduino pin A0. Voltages greater than 5v can also be accepted, but you will need to pass it through a voltage divider first (two resistors). NOTE: Whether you use this input or not, it should still also have a resistor added between pin A0 and ground, otherwise you will experience random firing. See the attached schematic PDF.
  * Automatically send repair signal when hit received - in the A_Setup.h file in the Arduino IDE, set both REPAIR_TANK and REPAIR_ON_HIT to "true". Any time a hit is received, a repair signal will be returned.

## "Apple" (IR receiver)
You can connect a standard Tamiya "apple" directly to your Arduino (Tamiya 53447). The Tamiya apple combines an IR receiver and notification LEDs; the LEDs already have current limiting resistors included inside the apple.

If you want to build your own IR receiver and use your own hit notification LEDs, you need to include your own current limiting resistor appropriate to the notification LEDs you choose. Most Arduinos can't source more than 40mA per pin.

## IR Emitter
For the IR transmitter you can use the Tamiya IR LED that is included with the Tamiya apple, or a Taigen/Heng Long IR LED.

We have also found the [Vishay TSAL6100 (DigiKey 751-1203-ND)](https://www.digikey.com/en/products/detail/TSAL6100/751-1203-ND/1681338) to be a comparable replacement to the Tamiya.

For maximum distance the IR LED needs to be driven far beyond its typical current rating, but even so it still needs a current limiting resistor. The LED will survive the high current because the IR signal is very brief. In testing we have found a 3.3 ohm, 1 watt resistor to be the best compromise between range and LED longevity.

If you wish to send repair signals it is often desired to prevent the beam from traveling very far. In this case a higher value resistor is used inline with the IR emitter - we have found 1k ohm will give you a range of just a few feet.

## Recoil Servo
Attach the signal wire of your recoil servo to Arduino pin D8. The servo will perform a recoil effect movement when the cannon is fired. Recoil servo adjustments (end points, reverse, retract and return times) can be set using the options at the top of the A_Setup.h file.

## Taigen or similar Flash unit
A Taigen cannon flash unit can be used and will be flashed when the cannon is fired. Connect the flash signal wire to Arduino pin D6.

## Sound
Up to 4 sounds can be added with the use of an Adafruit "Audio FX" board. They have several versions, you will want to use the ones with a built-in amplifer. They have a [2 MB version](https://www.adafruit.com/product/2210) and a [16 MB](https://www.adafruit.com/product/2217) version.

These are the four sounds that can be played, add them to your Audio board with these names:
T01.wav - cannon fire sound
T02.wav - cannon "hit" sound
T03.wav - destroyed sound (device has received enough hits to be destroyed)
T04.wav - repair sound

These sounds will play when the cannon is fired, a cannon hit is received, the vehicle is destroyed, or a repair operation occurs, respectively.

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

You don't have to add all four sounds if you don't want. Also make sure the Audio board and your Arduino share a common Ground connection, and of course both have power.

Here is [Adafruit's Tutorial](https://learn.adafruit.com/adafruit-audio-fx-sound-board/overview) on these boards for more information.


# Firmware Notes

## Compiling Firmware
The Arduino project has made and continues to make all sorts of changes to their compiler, and unfortunately this can sometimes cause problems, and this proejct is one of those cases. The code will compile but will not always be stable or operational. If a non-compatible version of the Arduino IDE is used, the most common sign that problems will occur is if you see see a compile message to the effect that "global memory usage is over 90%," with the warning "Low memory available, stability problems may occur." And indeed, stability problems will occur.

Here is the workaround:

Load this project in the Arduino IDE. Go to the Tools menu -> Board -> Arduino AVR Boards -> select "Arduino Nano". Note that this will still work with a UNO even though you select Nano.

Now once again go to the Tools menu and select Board -> Boards Manager. Wait for the Boards information to load, then find the "Arduino AVR Boards" section. Click the "Select Version" drop-down box and select version 1.6.20 (not 1.6.2) and then click Install. After installation has completed, click Close to close the Boards Manager, and then compile.

The compiled code size should show global memory usage somewhere in the 70% range with no warning about instability problems. [See this thread](https://openpanzer.org/forum/index.php?topic=19.0) for a further dicussion of this issue. 

# Firmware Settings
Within the sketch is a file called "A_Setup.h" (it will appear as a tab in your Arduino editor). That is where all user settings can be adjusted. Recoil servo, IR protocol selection, "weight class" and other settings are defined here. There are extensive notes in this file so just read through it and it should be self-explanatory. 

# Troubleshooting
Leave your Arduino attached to your computer with a USB cable and open the Serial Monitor from within the Arduino IDE to see informational messages printed during operation.


## Example project
See this thread over at RC Tank Warfare where this project is interfaced with a standard Heng Long board to add Tamiya IR compatibility: [Arduino UNO IR Battle System](https://www.rctankwarfare.co.uk/forums/viewtopic.php?f=81&t=21941).

See also this RC Tank Warfare thread: [How to build a working IR Artillery Gun](https://www.rctankwarfare.co.uk/forums/viewtopic.php?f=208&t=22849)

## License
All Open Panzer firmware is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

For more specific details see [http://www.gnu.org/licenses](http://www.gnu.org/licenses), the [Quick Guide to GPLv3.](http://www.gnu.org/licenses/quick-guide-gplv3.html) and the [copying.txt](https://github.com/OpenPanzerProject/TCB/blob/master/COPYING.txt) file in the codebase.

The GNU operating system which is under the same license has an informative [FAQ here](http://www.gnu.org/licenses/gpl-faq.html).
