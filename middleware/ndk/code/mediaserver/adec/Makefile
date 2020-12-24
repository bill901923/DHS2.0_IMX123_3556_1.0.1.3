LOCAL_DIR := $(shell pwd)
NDK_PATH := $(LOCAL_DIR)/../../..
MC_PARAM_FILE:=../Makefile.param
include $(MC_PARAM_FILE)

#*************************************************************************
# all source file in this module
SRCS := mapi_adec.c

-include $(MEDIASERVER_PATH)/adapt/adec/aac/Makefile

CFLAGS += -I$(MEDIASERVER_PATH)/adapt/adec/include
CFLAGS += -I$(MEDIASERVER_PATH)/adapt/adec/aac/
#*************************************************************************
TARGET := $(HIARCH)_mapi_adec

#*************************************************************************
# compile linux or HuaweiLite
include $(PWD)/../Make.$(OSTYPE)
