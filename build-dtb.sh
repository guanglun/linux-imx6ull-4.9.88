#!/bin/sh

make ARCH=arm CROSS_COMPILE=$TARGET_PREFIX imx6ull-xly-emmc-lcd.dtb
make ARCH=arm CROSS_COMPILE=$TARGET_PREFIX imx6ull-xly-emmc-hdmi.dtb
