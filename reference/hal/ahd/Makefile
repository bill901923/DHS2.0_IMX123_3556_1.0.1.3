# Module build makefile
CUR_ROOT    := $(shell pwd)
REF_ROOT    ?= $(CUR_ROOT)/../..
OS_TYPE     := HuaweiLite
include $(REF_ROOT)/build/base.mak

MODULE_ROOT := $(CUR_ROOT)
MODULE_NAME := hal$(CUR_DIR_NAME)
HARDWARE_TYPE:=
ifeq ($(CONFIG_AHD),y)
	SRC_ROOT    := $(MODULE_ROOT)
	SRC_DIR     := $(SRC_ROOT)/$(HARDWARE_TYPE)
	OBJ_ROOT    := $(REF_OUT_HAL_OBJ)/$(OS_TYPE)/$(MODULE_NAME)
	LIB_ROOT    := $(REF_OUT_HAL_LIB)/$(OS_TYPE)
	VSS_CFLAGS +=-D_GNU_SOURCE
	ifneq ($(CONFIG_AHD_N3),y)
		SRCS_OMIT:= $(SRC_ROOT)/n3/hal_ahd_n3.c
	endif
	ifneq  ($(CONFIG_AHD_NVP6324),y)
		SRCS_OMIT += $(SRC_ROOT)/nvp6324/hal_ahd_nvp6324.c
	endif
	include $(REF_ROOT)/build/modbuild.mak
endif