CUR_DIR  := $(shell pwd)
SDK_ROOT ?= $(CUR_DIR)/../../../..
-include  $(SDK_ROOT)/build/base.mak
dummy := $(call CreateDir, $(DRV_REL_PATH))

MODEL         ?= sdio_rtl8189ftv
MODULE_PATH          ?= $(CUR_DIR)
export ARCH          ?= $(OSDRV_ARCH)
export CROSS_COMPILE ?= $(LINUX_CROSS)-
KERNEL          ?= $(LINUX_ROOT)
PREFIX          ?= $(DRV_REL_PATH)/wifi/sdio_rtl8189ftv
FIRMWARE_PATH          :=$(CUR_DIR)/firmware/$(MODEL)
################################################################################
ARCH		?=arm
CROSS_COMPILE   ?=arm-himix100-linux-

MKFLAGS  := ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)	KSRC=/home/xll/workspace/DHS2.0_IMX123_3556_1.0.1.3/osdrv/opensource/kernel/linux-4.9.y
################################################################################
BUILD_DIR       := $(shell pwd)/tmp
SRC             := rtl8189FS_linux_v5.3.16_32695.20190327
KMODS		:= 8189fs.ko
TARGETS		:= $(addprefix $(PREFIX)/$(KLIB)/,$(KMODS))
################################################################################
all: $(TARGETS)

$(BUILD_DIR)/$(SRC)/%.ko: $(BUILD_DIR)/$(SRC)
	make -C $(BUILD_DIR)/$(SRC) $(MKFLAGS) -j32

$(PREFIX)/$(KLIB)/%.ko: $(BUILD_DIR)/$(SRC)/%.ko
	@test -d $(PREFIX)/$(KLIB) || mkdir -p $(PREFIX)/$(KLIB)
	@cp -v $(BUILD_DIR)/$(SRC)/$(KMODS) $(PREFIX)/$(KLIB)

$(BUILD_DIR)/$(SRC):
	test -d $(BUILD_DIR) || mkdir -p $(BUILD_DIR)
	tar -zxf $(SRC).tar.gz -C $(BUILD_DIR)

clean:
	-rm -rf $(BUILD_DIR)
	-rm -rf $(TARGETS)

distclean: clean

################################################################################
.PHONY: clean distclean
################################################################################
