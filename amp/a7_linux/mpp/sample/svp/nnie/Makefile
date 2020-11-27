# Hisilicon Hi35xx sample Makefile
include $(PWD)/../Makefile.param
ifeq ($(ARM_ARCH), amp)
ifeq ($(OSTYPE), linux)
CFLAGS += -I$(PWD)/sample
CFLAGS += -I$(PWD)/sample_nnie_software
CFLAGS += -O3
#CFLAGS  += -DSAMPLE_SVP_NNIE_PERF_STAT

AMP_CLIENT_SRCS := $(wildcard *.c)
AMP_CLIENT_SRCS += $(wildcard ./sample/*.c)
AMP_CLIENT_SRCS += $(wildcard ./sample_nnie_software/*.c)
AMP_CLIENT_SRCS += $(wildcard $(PWD)/../common/*.c)

TARGET := sample_nnie_main

TARGET_PATH := $(PWD)

# compile linux or HuaweiLite
include $(PWD)/../../$(ARM_ARCH)_$(OSTYPE).mak
endif
endif
