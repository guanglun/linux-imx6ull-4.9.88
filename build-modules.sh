#!/bin/sh

make ARCH=arm CROSS_COMPILE=$TARGET_PREFIX LOCALVERSION="" modules

rm ./modules_install -rf
make ARCH=arm CROSS_COMPILE=$TARGET_PREFIX modules_install INSTALL_MOD_PATH=./modules_install

cd ./modules_install/lib/modules
tar cvjf ../../modules.tar.bz2 ./

