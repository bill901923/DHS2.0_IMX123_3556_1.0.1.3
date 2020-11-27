####### select serdes type ##################
####    241a242            ##################

SERDES_TYPE = 241a242

ifeq ($(EXTDRV_PARAM_FILE), )
    EXTDRV_PARAM_FILE:=../Makefile.param
    include $(EXTDRV_PARAM_FILE)
endif

SRCS := serdes_comm.c

ifeq ($(SERDES_TYPE), 241a242)
SRCS += thine_241a242_serdes.c
endif

INC += -I$(REL_INC)
INC += -I$(OSAL_ROOT)/include
ifeq ($(MPP_BUILD),y)
        INC += -I$(OSAL_ROOT)/$(OSTYPE)/kernel/himedia
else
        INC += -I$(OSAL_ROOT)/source/kernel/himedia
        INC += -I$(MPP_PATH)/include
endif
EXTDRV_CFLAGS += $(INC)

#*************************************************************************
TARGET := hi_serdes

#*************************************************************************
# compile linux or HuaweiLite
include $(PWD)/../Make.$(OSTYPE)

