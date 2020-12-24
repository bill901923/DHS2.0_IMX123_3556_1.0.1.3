# Module build makefile
CUR_ROOT    := $(shell pwd)
REF_ROOT    ?= $(CUR_ROOT)/../..
OS_TYPE     := Linux
include $(REF_ROOT)/build/base.mak

MODULE_ROOT := $(CUR_ROOT)
MODULE_NAME := hal$(CUR_DIR_NAME)
HARDWARE_TYPE:= rtl8189
ifeq ($(CONFIG_WIFI),y)
	ifeq ($(CONFIG_WIFI_RTL8189),y)
		HARDWARE_TYPE:= rtl8189
	endif
	SRC_ROOT    := $(MODULE_ROOT)/src
	SRC_DIR     := $(SRC_ROOT)/common/
	SRC_DIR     += $(SRC_ROOT)/$(HARDWARE_TYPE)/
	OBJ_ROOT    := $(REF_OUT_HAL_OBJ)/$(OS_TYPE)/$(MODULE_NAME)
	LIB_ROOT    := $(REF_OUT_HAL_LIB)/$(OS_TYPE)
	VSS_CFLAGS +=-D_GNU_SOURCE

	include $(REF_ROOT)/build/modbuild.mak
endif