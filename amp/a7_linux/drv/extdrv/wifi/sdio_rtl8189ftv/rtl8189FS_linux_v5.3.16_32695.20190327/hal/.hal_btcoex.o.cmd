cmd_/home/xll/workspace/NEW_Hi3559V200_MobileCam_SDK_V1.0.1.3/amp/a7_linux/drv/extdrv/wifi/sdio_rtl8189ftv/rtl8189FS_linux_v5.3.16_32695.20190327/hal/hal_btcoex.o := arm-himix100-linux-gcc -Wp,-MD,/home/xll/workspace/NEW_Hi3559V200_MobileCam_SDK_V1.0.1.3/amp/a7_linux/drv/extdrv/wifi/sdio_rtl8189ftv/rtl8189FS_linux_v5.3.16_32695.20190327/hal/.hal_btcoex.o.d  -nostdinc -isystem /opt/hisi-linux/x86-arm/arm-himix100-linux/host_bin/../lib/gcc/arm-linux-uclibceabi/6.3.0/include -I./arch/arm/include -I./arch/arm/include/generated/uapi -I./arch/arm/include/generated  -I./include -I./arch/arm/include/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-hibvt/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -std=gnu89 -fno-PIE -fno-dwarf2-cfi-asm -fno-ipa-sra -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -fno-delete-null-pointer-checks -Wno-frame-address -Os -Wno-maybe-uninitialized --param=allow-store-data-races=0 -DCC_HAVE_ASM_GOTO -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -Wno-unused-const-variable -fomit-frame-pointer -fno-var-tracking-assignments -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Werror=implicit-int -Werror=strict-prototypes -Werror=date-time -Werror=incompatible-pointer-types -O1 -Wno-unused-variable -Wno-unused-value -Wno-unused-label -Wno-unused-parameter -Wno-unused-function -Wno-unused -Wno-date-time -I/home/xll/workspace/NEW_Hi3559V200_MobileCam_SDK_V1.0.1.3/amp/a7_linux/drv/extdrv/wifi/sdio_rtl8189ftv/rtl8189FS_linux_v5.3.16_32695.20190327/include -I/home/xll/workspace/NEW_Hi3559V200_MobileCam_SDK_V1.0.1.3/amp/a7_linux/drv/extdrv/wifi/sdio_rtl8189ftv/rtl8189FS_linux_v5.3.16_32695.20190327/platform -I/home/xll/workspace/NEW_Hi3559V200_MobileCam_SDK_V1.0.1.3/amp/a7_linux/drv/extdrv/wifi/sdio_rtl8189ftv/rtl8189FS_linux_v5.3.16_32695.20190327/hal/btc -DCONFIG_RTL8188F -DCONFIG_MP_INCLUDED -DCONFIG_POWER_SAVING -DCONFIG_EFUSE_CONFIG_FILE -DEFUSE_MAP_PATH=\"/system/etc/wifi/wifi_efuse_8189fs.map\" -DWIFIMAC_PATH=\"/data/wifimac.txt\" -DCONFIG_LOAD_PHY_PARA_FROM_FILE -DREALTEK_CONFIG_PATH=\"/lib/firmware/\" -DCONFIG_TXPWR_BY_RATE=1 -DCONFIG_TXPWR_BY_RATE_EN=1 -DCONFIG_TXPWR_LIMIT=1 -DCONFIG_TXPWR_LIMIT_EN=0 -DCONFIG_RTW_ADAPTIVITY_EN=0 -DCONFIG_RTW_ADAPTIVITY_MODE=0 -DCONFIG_RTW_SDIO_PM_KEEP_POWER -DCONFIG_REDUCE_TX_CPU_LOADING -DCONFIG_BR_EXT '-DCONFIG_BR_EXT_BRNAME="'br0'"' -DCONFIG_RTW_NAPI -DCONFIG_RTW_GRO -DCONFIG_RTW_NETIF_SG -DCONFIG_RTW_DEBUG -DRTW_LOG_LEVEL=4 -DDM_ODM_SUPPORT_TYPE=0x04 -DCONFIG_LITTLE_ENDIAN -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT -I/home/xll/workspace/NEW_Hi3559V200_MobileCam_SDK_V1.0.1.3/amp/a7_linux/drv/extdrv/wifi/sdio_rtl8189ftv/rtl8189FS_linux_v5.3.16_32695.20190327/hal/phydm  -DMODULE  -DKBUILD_BASENAME='"hal_btcoex"'  -DKBUILD_MODNAME='"8189fs"' -c -o /home/xll/workspace/NEW_Hi3559V200_MobileCam_SDK_V1.0.1.3/amp/a7_linux/drv/extdrv/wifi/sdio_rtl8189ftv/rtl8189FS_linux_v5.3.16_32695.20190327/hal/hal_btcoex.o /home/xll/workspace/NEW_Hi3559V200_MobileCam_SDK_V1.0.1.3/amp/a7_linux/drv/extdrv/wifi/sdio_rtl8189ftv/rtl8189FS_linux_v5.3.16_32695.20190327/hal/hal_btcoex.c

source_/home/xll/workspace/NEW_Hi3559V200_MobileCam_SDK_V1.0.1.3/amp/a7_linux/drv/extdrv/wifi/sdio_rtl8189ftv/rtl8189FS_linux_v5.3.16_32695.20190327/hal/hal_btcoex.o := /home/xll/workspace/NEW_Hi3559V200_MobileCam_SDK_V1.0.1.3/amp/a7_linux/drv/extdrv/wifi/sdio_rtl8189ftv/rtl8189FS_linux_v5.3.16_32695.20190327/hal/hal_btcoex.c

deps_/home/xll/workspace/NEW_Hi3559V200_MobileCam_SDK_V1.0.1.3/amp/a7_linux/drv/extdrv/wifi/sdio_rtl8189ftv/rtl8189FS_linux_v5.3.16_32695.20190327/hal/hal_btcoex.o := \
    $(wildcard include/config/bt/coexist.h) \
    $(wildcard include/config/lps/lclk.h) \
    $(wildcard include/config/mcc/mode.h) \
    $(wildcard include/config/p2p.h) \
    $(wildcard include/config/bt/coexist/socket/trx.h) \
    $(wildcard include/config/rf4ce/coexist.h) \
    $(wildcard include/config/rtl8192e.h) \
    $(wildcard include/config/rtl8821a.h) \
    $(wildcard include/config/rtl8723b.h) \
    $(wildcard include/config/rtl8812a.h) \
    $(wildcard include/config/rtl8703b.h) \
    $(wildcard include/config/rtl8822b.h) \
    $(wildcard include/config/rtl8723d.h) \
    $(wildcard include/config/rtl8821c.h) \
    $(wildcard include/config/pci/hci.h) \
    $(wildcard include/config/usb/hci.h) \
    $(wildcard include/config/sdio/hci.h) \
    $(wildcard include/config/gspi/hci.h) \
    $(wildcard include/config/fw/multi/port/support.h) \
    $(wildcard include/config/load/phy/para/from/file.h) \

/home/xll/workspace/NEW_Hi3559V200_MobileCam_SDK_V1.0.1.3/amp/a7_linux/drv/extdrv/wifi/sdio_rtl8189ftv/rtl8189FS_linux_v5.3.16_32695.20190327/hal/hal_btcoex.o: $(deps_/home/xll/workspace/NEW_Hi3559V200_MobileCam_SDK_V1.0.1.3/amp/a7_linux/drv/extdrv/wifi/sdio_rtl8189ftv/rtl8189FS_linux_v5.3.16_32695.20190327/hal/hal_btcoex.o)

$(deps_/home/xll/workspace/NEW_Hi3559V200_MobileCam_SDK_V1.0.1.3/amp/a7_linux/drv/extdrv/wifi/sdio_rtl8189ftv/rtl8189FS_linux_v5.3.16_32695.20190327/hal/hal_btcoex.o):
