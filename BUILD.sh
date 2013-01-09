#!/bin/sh

CROSS=/usr/obj/arm.armv6/usr/home/ray/work/FreeBSD/Projects/Efika_MX/src/efika_mx/tmp/usr/bin/

gmake ARCH=arm CROSS_COMPILE=${CROSS} distclean || exit 1
gmake ARCH=arm CROSS_COMPILE=${CROSS} efikasb_config || exit 1
gmake ARCH=arm CROSS_COMPILE=${CROSS} USE_PRIVATE_LIBGCC=yes || exit 1

#echo cp u-boot.bin /tftpboot/efika_mx/u-boot_efika.bin
#cp u-boot.bin /tftpboot/efika_mx/u-boot_efika.bin

echo sudo dd if=u-boot.imx of=/dev/da0 bs=1k seek=1 conv=sync