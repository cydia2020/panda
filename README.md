smartDSU 
======
smartDSU is kind of like WINE in that it is NOT an Emulator.

It sits between your DSU and the powertrain CAN bus of Toyota vehicles that have a DSU and filters the traffic between them, enabling openpilot longitudinal control without removing AEB. It also prevents cruise fault in case the Eon boots up after the car for some reason. The cars known at the time of this writing are listed below:

|   Year   |   Model   |
|:----     |   :----   |
|2017-2019 |Corolla    |
|2016-2018 |Rav4       |
|2016-2018 |Rav4 Hybrid|
|2016-2020 |Prius      |
|2017-2020 |Prius Prime|
|2017-2019 |Highlander |
|2018-2019 |Sienna     |

There are probably others that I didn't list, but you will know if your car has a DSU or not.

How it Works
----
1) The Panda boots into AllOutput mode, where it forwards all the traffic between bus2 and bus0. Once there is traffic on bus0, it emits the ID message (0x2FF, 4 bytes) on bus 0.
2) As soon as CAN messages begin showing up on bus2, a timer is started for 2 seconds.
3) During this time, smartDSU filters out 0x343 and sends a spoofed version to bus 0, which disables the low speed lockout.
4) After the timer exceeds 2 seconds, 0x343 from the DSU is filtered if it is detected on bus0 (meaning the Eon is sending it.) If it is not detected on bus0, then it is forwarded from the DSU.

Bugs
----
- If the Eon is on before the car is on, smartDSU will send the spoof message for the first 2 seconds while the Eon also sends 0x343. This has no effect on the car, but should still be fixed (it's pretty easy)
- The DSU will think it can engage at any speed, and it will temporarily throw an error on the dash if the car does not support full-speed radar cruise and cruise is engaged below 25mph. This did not lock out the 2018 Corolla it was tested on.

Flashing instructions
----
### From Mac / Linux:
 Just follow the regular [flashing guide.](board/README.md)
### From an Eon (with Toyota Giraffe or Debug Board):
 Plug the Eon into the Panda. Plug a 12v DC source into the DC jack of the Giraffe / Debug Board and plug the Panda into it, setting Giraffe switches to 0101 or turning OFF the IGN switch on the debug board. SSH into the Eon and run the following commands, one-by-one, in order:
 ```
 cd /data
 git clone -b smart_dsu https://github.com/wocsor/panda.git smart_dsu
 cd smart_dsu/board/tools && killall boardd
 ./enter_download_mode.py
 make recover
 reboot
 ```
### From Windows:
 You will need pre-compiled binaries. I put them in the BIN folder but if you'd like to compile the firmware yourself you have 2 options:
 - Get someone with Linux to compile the firmware and send bootstub.panda.bin and panda.bin to you
 - Use WSL on Windows 10, following the Linux instructions. Instead of running 
 `make` run `make recover` and your bin files will be in the board\obj folder.
 
 Next, you will need to install the WinUSB driver with [Zadig.](https://github.com/pbatard/libwdi/releases/download/b721/zadig-2.4.exe)
 Use a Paw or some other means to get your Panda into DFU mode. If STM BOOTLOADER does not populate in the dropdown list, you may need to click Options > List All Devices. Click Install Driver.

 You will now need the Windows version of dfu-util so you can flash the Panda with the bin files. It can be downloaded [here.](http://dfu-util.sourceforge.net/releases/dfu-util-0.8-binaries/win32-mingw32/dfu-util-static.exe
 )

 Place dfu-util and the bins into the same folder and navigate to it in Command Prompt. Run these two commands in order:
 ```
 dfu-util-static.exe -d 0483:df11 -a 0 -s 0x08004000 -D panda.bin
 dfu-util-static.exe -d 0483:df11 -a 0 -s 0x08000000:leave -D bootstub.panda.bin
 ```
The firmware should now be flashed. You will see a red/orange/yellow pulsating light indicating that the flash was successful.

### Hardware
You will need a white or grey Panda and a Toyota Giraffe. Make sure your Toyota Giraffe has the 120 ohm resistor between pins 12 and 13 of the OBD socket. You can test this with a multimeter. If it isn't installed internally you can add one manually by following [this guide.](https://medium.com/@comma_ai/toyota-giraffe-configuration-fix-20283ba4d01a)

STL files for the connectors are available in the STL folder if you would like to build one of these yourself, and the BOM is below:

|Part                                      |QTY     |Link   |
|------------------------------------------|:------:|-------|
|Dupont Connector Pins - Male + Female Pair|9m, 9f  | https://www.amazon.com/Gikfun-Female-Connector-Terminal-2-54mm/dp/B0146DJR9Q/ref=sr_1_6?keywords=dupont+male&qid=1579535904&sr=8-6 |
|TE Connector Pins - Male                  | 9 | https://www.digikey.com/product-detail/en/te-connectivity-amp-connectors/1376109-1/A124380CT-ND/6052257 |
|TE Connector Pins - Female                | 9 | https://www.digikey.com/product-detail/en/te-connectivity-amp-connectors/1123343-1/A107011CT-ND/3488572 |
|TE AMP Connector - Female        | 1 | https://www.digikey.com/product-detail/en/te-connectivity-amp-connectors/1318774-1/A122296-ND/2273519 |
|TE AMP Connector - Male          | 1 | https://www.digikey.com/products/en?keywords=1565894-1 |


TODO: create some information about the wiring of the harness.

Welcome to panda
======

[panda](http://github.com/commaai/panda) is the nicest universal car interface ever.

<a href="https://comma.ai/shop/products/panda-obd-ii-dongle"><img src="https://github.com/commaai/panda/blob/master/panda.png">

<img src="https://github.com/commaai/panda/blob/master/buy.png"></a>

It supports 3x CAN, 2x LIN, and 1x GMLAN. It also charges a phone. On the computer side, it has both USB and Wi-Fi.

It uses an [STM32F413](http://www.st.com/en/microcontrollers/stm32f413-423.html?querycriteria=productId=LN2004) for low level stuff and an [ESP8266](https://en.wikipedia.org/wiki/ESP8266) for Wi-Fi. They are connected over high speed SPI, so the panda is actually capable of dumping the full contents of the busses over Wi-Fi, unlike every other dongle on amazon. ELM327 is weak, panda is strong.

It is 2nd gen hardware, reusing code and parts from the [NEO](https://github.com/commaai/neo) interface board.

[![CircleCI](https://circleci.com/gh/commaai/panda.svg?style=svg)](https://circleci.com/gh/commaai/panda)

Usage (Python)
------

To install the library:
```
# pip install pandacan
```

See [this class](https://github.com/commaai/panda/blob/master/python/__init__.py#L80) for how to interact with the panda.

For example, to receive CAN messages:
```
>>> from panda import Panda
>>> panda = Panda()
>>> panda.can_recv()
```
And to send one on bus 0:
```
>>> panda.can_send(0x1aa, "message", 0)
```
Find user made scripts on the [wiki](https://community.comma.ai/wiki/index.php/Panda_scripts)

Note that you may have to setup [udev rules](https://community.comma.ai/wiki/index.php/Panda#Linux_udev_rules) for Linux, such as
```
sudo -i
echo 'SUBSYSTEMS=="usb", ATTR{idVendor}=="bbaa", ATTR{idProduct}=="ddcc", MODE:="0666"' > /etc/udev/rules.d/11-panda.rules
exit
```

Usage (JavaScript)
-------

See [PandaJS](https://github.com/commaai/pandajs)


Software interface support
------

As a universal car interface, it should support every reasonable software interface.

- [User space](https://github.com/commaai/panda/tree/master/python)
- [socketcan in kernel](https://github.com/commaai/panda/tree/master/drivers/linux) (alpha)
- [ELM327](https://github.com/commaai/panda/blob/master/boardesp/elm327.c)
- [Windows J2534](https://github.com/commaai/panda/tree/master/drivers/windows)

Directory structure
------

- board      -- Code that runs on the STM32
- boardesp   -- Code that runs on the ESP8266
- drivers    -- Drivers (not needed for use with python)
- python     -- Python userspace library for interfacing with the panda
- tests      -- Tests and helper programs for panda

Programming (over USB)
------

[Programming the Board (STM32)](board/README.md)

[Programming the ESP](boardesp/README.md)


Debugging
------

To print out the serial console from the STM32, run tests/debug_console.py

To print out the serial console from the ESP8266, run PORT=1 tests/debug_console.py

Safety Model
------

When a panda powers up, by default it's in `SAFETY_NOOUTPUT` mode. While in no output mode, the buses are also forced to be silent. In order to send messages, you have to select a safety mode. Currently, setting safety modes is only supported over USB.

Safety modes optionally supports `controls_allowed`, which allows or blocks a subset of messages based on a customizable state in the board.

Code Rigor
------
When compiled from an [EON Dev Kit](https://comma.ai/shop/products/eon-gold-dashcam-devkit), the panda FW is configured and optimized (at compile time) for its use in
conjuction with [openpilot](https://github.com/commaai/openpilot). The panda FW, through its safety model, provides and enforces the
[openpilot Safety](https://github.com/commaai/openpilot/blob/devel/SAFETY.md). Due to its critical function, it's important that the application code rigor within the `board` folder is held to high standards.

These are the [CI regression tests](https://circleci.com/gh/commaai/panda) we have in place:
* A generic static code analysis is performed by [Cppcheck](https://github.com/danmar/cppcheck/).
* In addition, [Cppcheck](https://github.com/danmar/cppcheck/) has a specific addon to check for [MISRA C:2012](https://www.misra.org.uk/MISRAHome/MISRAC2012/tabid/196/Default.aspx) violations. See [current coverage](https://github.com/commaai/panda/blob/master/tests/misra/coverage_table).
* Compiler options are relatively strict: the flags `-Wall -Wextra -Wstrict-prototypes -Werror` are enforced on board and pedal makefiles.
* The [safety logic](https://github.com/commaai/panda/tree/master/board/safety) is tested and verified by [unit tests](https://github.com/commaai/panda/tree/master/tests/safety) for each supported car variant.
* A recorded drive for each supported car variant is [replayed through the safety logic](https://github.com/commaai/panda/tree/master/tests/safety_replay)
to ensure that the behavior remains unchanged.
* An internal Hardware-in-the-loop test, which currently only runs on pull requests opened by comma.ai's organization members, verifies the following functionalities:
    * compiling the code in various configuration and flashing it both through USB and WiFi.
    * Receiving, sending and forwarding CAN messages on all buses, over USB and WiFi.

In addition, we run [Pylint](https://www.pylint.org/) and [Flake8](https://github.com/PyCQA/flake8) linters on all python files within the panda repo.

Hardware
------

Check out the hardware [guide](https://github.com/commaai/panda/blob/master/docs/guide.pdf)

Licensing
------

panda software is released under the MIT license unless otherwise specified.
