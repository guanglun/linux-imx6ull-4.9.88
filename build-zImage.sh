#!/bin/sh
rm include/config/kernel.release
make ARCH=arm CROSS_COMPILE=$TARGET_PREFIX LOCALVERSION="" zImage -j8
