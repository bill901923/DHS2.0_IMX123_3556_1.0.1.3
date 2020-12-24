#!/bin/sh

# path configure
sdk_build_path=`pwd`
sdk_path=`pwd`/..

mpp_liteos_path=${sdk_path}/amp/a7_liteos

source ${sdk_path}/.config

serdes_enable=n
if [ "${CONFIG_SNS0_SERDES}" == "y" ]; then
	serdes_enable=y
elif [ "${CONFIG_SNS1_SERDES}" == "y" ]; then
	serdes_enable=y
fi
if [ "${serdes_enable}" == "y" ]; then
	echo serdes enable
	sed -i 's;^\/\/#define HI_SUPPORT_SERDES;#define HI_SUPPORT_SERDES;g' ${mpp_liteos_path}/drv/interdrv/mipi_rx/mipi_rx_hal.c
else
	echo serdes disable
	sed -i 's;^#define HI_SUPPORT_SERDES;\/\/#define HI_SUPPORT_SERDES;g' ${mpp_liteos_path}/drv/interdrv/mipi_rx/mipi_rx_hal.c
fi

