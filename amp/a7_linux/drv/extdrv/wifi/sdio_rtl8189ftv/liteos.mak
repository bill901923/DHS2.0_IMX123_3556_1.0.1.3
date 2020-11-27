CUR_DIR  := $(shell pwd)
SDK_ROOT ?= $(CUR_DIR)/../../../../../../
include $(SDK_ROOT)/build/base.mak
dummy    := $(call CreateDir, $(DRV_REL_PATH_LITEOS))


export LITEOSTOPDIR ?= $(LITEOS_ROOT)
export WIFI_DEVICE  ?= sdio_rtl8189ftv
################################################################################
BUILD_DIR       := $(CUR_DIR)/tmp
SRC             := rtl8189FS_linux_v5.3.16_32695.20190327

all:
ifeq ($(AMP_TYPE),liteos)
	test -d $(BUILD_DIR) || mkdir -p $(BUILD_DIR)
	tar -zxf $(SRC).tar.gz -C $(BUILD_DIR)
	make driver -C $(BUILD_DIR)/$(SRC) >/dev/null
	make tools -C $(BUILD_DIR)/$(SRC) >/dev/null
	#make sample -C $(BUILD_DIR)/$(SRC) >/dev/null
	cp -v $(BUILD_DIR)/$(SRC)/out/$(CFG_CHIP_TYPE)/lib/*.a $(DRV_REL_PATH_LITEOS)/
endif

clean:
ifeq ($(AMP_TYPE),liteos)
	-make clean -C $(BUILD_DIR)/$(SRC)
endif

.PHONY: all clean


