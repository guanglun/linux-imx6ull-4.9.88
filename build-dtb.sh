#!/bin/sh

make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- imx6ull-xly-emmc-lcd.dtb
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- imx6ull-xly-emmc-hdmi.dtb
