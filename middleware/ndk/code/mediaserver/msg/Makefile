LOCAL_DIR := $(shell pwd)
NDK_PATH := $(LOCAL_DIR)/../../..
MC_PARAM_FILE:=../Makefile.param
include $(MC_PARAM_FILE)
include $(NDK_PATH)/build/config_$(HIARCH).mak
#*************************************************************************
# all source file in this module
SRCS := msg_server.c
SRCS += msg_wrapper.c
SRCS += msg_sys.c
SRCS += msg_vcap.c
SRCS += msg_venc.c
SRCS += msg_vproc.c
SRCS += msg_disp.c
ifeq ($(CFG_SUPPORT_HDMI), y)
SRCS += msg_hdmi.c
endif
SRCS += msg_acap.c
SRCS += msg_aenc.c
ifeq ($(CFG_SUPPORT_ADEC), y)
SRCS += msg_adec.c
endif
SRCS += msg_ao.c

#SRCS += msg_pm.c

#*************************************************************************
TARGET := $(HIARCH)_media_server

CFLAGS += -I$(MEDIASERVER_PATH)/adapt/audio/include
CFLAGS += -I$(MEDIASERVER_PATH)/comm/arch/$(HIARCH)
CFLAGS += -I$(MEDIASERVER_PATH)/venc/os/include

#*************************************************************************
# compile linux or HuaweiLite
include $(PWD)/../Make.$(OSTYPE)
