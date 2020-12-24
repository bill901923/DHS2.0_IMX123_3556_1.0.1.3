# Module build makefile
CUR_ROOT    := $(shell pwd)
# $(CUR_ROOT)/..  => /core
# $(CUR_ROOT)/../..  => /scene
# $(CUR_ROOT)/../../..  => /module
# $(CUR_ROOT)/../../../..  => /dashcam
PDT_ROOT    ?= $(CUR_ROOT)/../../../..
OS_TYPE     := HuaweiLite
include $(PDT_ROOT)/build/base.mak

MODULE_ROOT := $(CUR_ROOT)/..
MODULE_NAME := scene_server


SRC_ROOT    := $(REF_ROOT)
SRC_DIR     := $(CUR_ROOT)


SRCS_BACK    := $(shell find $(SRC_DIR) -name 'hi_product_scene_setparam_hi35*.c')
SRCS_BACK    := $(sort $(SRCS_BACK))
SRC_USE      := $(CUR_ROOT)/hi_product_scene_setparam_$(CFG_CHIP_TYPE).c
SRCS_OMIT    := $(filter-out $(SRC_USE),$(SRCS_BACK))
ifneq ($(AMP_TYPE), linux_liteos)
SRCS_OMIT    += $(CUR_ROOT)/hi_product_scene_msg_server.c
endif

VSS_INC_PATH += -I$(CUR_ROOT)
VSS_INC_PATH += -I$(PDT_SCENE_INICFG_PATH)
VSS_INC_PATH += -I$(CUR_ROOT)/../include/$(CFG_CHIP_TYPE)
VSS_INC_PATH += -I$(PDT_ROOT)/../thirdparty/libjpeg-turbo-1.5.3/out/include
VSS_INC_PATH += -L$(PDT_ROOT)/../thirdparty/libjpeg-turbo-1.5.3/out/lib

OBJ_ROOT    := $(PDT_OUT_MODULES_OBJ)/$(OS_TYPE)/$(MODULE_NAME)
LIB_ROOT    := $(PDT_OUT_MODULES_LIB)/$(OS_TYPE)
LIB_TYPE    := static

PIPETYPE_CNT    = $(shell grep -nr "config_product_scene_*" $(PDT_SCENE_INICFG_PATH)/config_cfgaccess_entry.ini |wc -l)
VSS_MOD_CFLAGS := -DPDT_SCENE_PIPETYPE_CNT=$(PIPETYPE_CNT) 

include $(REF_ROOT)/build/modbuild.mak
