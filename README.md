# MultiOsc
----
## Description
Two-channel 7Khz to 200Mhz clock/oscillator.

----
## Overview
MultiOsc is a very cheap two-channel clock generator which can be configured to generate frequencies from 8Khz to 200Mhz (180 to 200, depends on the SI5153 version) for less than 10€.

The device is based on the SI5351 clock generator, more concretely using the module sold in AliExpress (for less than 4€) which is very convenient as it already contains all the required external components to get it working immediately.

The device uses a STM32F103 module to control the clock generator and drive a 0.66" I2C screen.

## Schematic

The schematic is extremely simple, the three modules are connected through the I2C bus and five push buttons are used to control de menus, nothing more nothing else :)

![Schematic picture](Schematic.jpg?raw=true "Schematic")

## PCB

I also have included a very basic PCB to construct the device but it's not needed at all. I have assembled it using a protoboard and it works really well, but for anyone who wants to make it more robust the PCB is a good option.

![ProtoBoard picture 1](ProtoBoard.jpg?raw=true "ProtoBoard 1")

![ProtoBoard picture 2](ProtoBoard2.jpg?raw=true "ProtoBoard 2")

![PCB picture](PCB.jpg?raw=true "PCB")

Both, schematic and PCB are included under the "PCB" folder, it's a KiCad 6.0 project and it includes all the custom footprints for the modules in case you want to reuse them.

## BOM

The bill of materials is very reduced:

* STM32F103 module:  https://es.aliexpress.com/item/1005002441720435.html
* 0.66" OLED screen: https://es.aliexpress.com/item/1005002760899272.html
* SI5351 module:     https://es.aliexpress.com/item/32915414654.html
* Push buttons (5):  https://es.aliexpress.com/item/1005004001434474.html

I am 99.99999% sure the STM module uses a clone uC, not an original STM device, but as far as I can tell the device works flawlessly.

## STM32F103 bootloader

I have seen a lot of people complaining about the F103 modules failing to run the bootloader, but this is all a missunderstanding on how the bootloader works for the F103.

If you check carefully the [AN2606](https://www.st.com/resource/en/application_note/cd00167594-stm32-microcontroller-system-memory-boot-mode-stmicroelectronics.pdf) it clarifies the problem and is really simple, the STM32F10XXX family only has UART bootloader and the modules connect the USB connector to the USB pins from these devices, there is no integrated serial-to-usb converter so the bootloader will not work connecting only the USB port, you also need a serial-to-usb converter and connect it to pins PA9 and PA10, with this the device will be accessible from STM32CubeProgrammer.

In any case I recommend to use a ST-LINK/V2 or similar (I have an original one and a clone, both work but the original one is less prone to cause errors and can be upgraded, so if you can, buy an original one), it works a lot faster and allows to debug the code in real time.

## Firmware

The firmware project is a STM32CubeIde project, currently I have used the 1.9.0 version and it uses the standard drivers and two heavily modified libraries.

The first library is the SSD1306 library by afiskon and you can find the original here: https://github.com/afiskon/stm32-ssd1306
The second library is the SI5351 library by MR-DOS and you can find the original here: https://github.com/MR-DOS/Si5351-lib

As you can imagine the firmware itself is very simple, it creates a menu where you can change the two output frequencies and that's all, nothing else is needed, and as it relies on the two aforementioned libraries controlling the screen and the SI module is a breeze.

Beware that I found there is a problem with CubeMX, if you open the CubeMX file to reconfigure the HAL drivers, pinout or whatever the CubeMX does break the I2C configuration, for the F103 device it does not allow to configure the port speed and without the configuration the I2C bus does not work. If you need to reconfigure the hardware, after re-generating the files you will need to modify the file "stm32f1xx_hal_msp.c" and add to the I2C initialization, before the "HAL_GPIO_Init" call, this: 

	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

---


Have fun!