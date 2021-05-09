#!/bin/sh

rm include/config/kernel.release

make ARCH=arm CROSS_COMPILE=$TARGET_PREFIX distclean

make ARCH=arm CROSS_COMPILE=$TARGET_PREFIX imx_xly_defconfig

make ARCH=arm CROSS_COMPILE=$TARGET_PREFIX LOCALVERSION="" zImage -j8
#make ARCH=arm CROSS_COMPILE=$TARGET_PREFIX LOCALVERSION="" all -j8

# dts
make ARCH=arm CROSS_COMPILE=$TARGET_PREFIX imx6ull-xly-emmc-lcd.dtb

#modules
make ARCH=arm CROSS_COMPILE=$TARGET_PREFIX LOCALVERSION="" modules

rm ./modules_install -rf
make ARCH=arm CROSS_COMPILE=$TARGET_PREFIX modules_install LOCALVERSION="" INSTALL_MOD_PATH=./modules_install

cd ./modules_install/lib/modules
tar cvjf ../../modules.tar.bz2 ./
