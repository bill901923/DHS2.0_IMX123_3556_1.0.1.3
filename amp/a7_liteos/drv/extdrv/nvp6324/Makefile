ifeq ($(EXTDRV_PARAM_FILE), )
	PARAM_FILE:=../../../mpp/Makefile.param
    include $(PARAM_FILE)
endif

SRCS :=   jaguar1_drv.c jaguar1_i2c.c jaguar1_video.c jaguar1_coax_protocol.c jaguar1_motion.c jaguar1_video_eq.c jaguar1_mipi.c

INC += -I$(OSAL_ROOT)/linux/kernel/himedia
INC += -I$(OSAL_ROOT)/include
INC += -I$(REL_INC)

##EXTRA_CFLAGS += -I$(REL_INC)
INC += -I$(DRV_ROOT)/extdrv/$(HIARCH)/nvp6324/

EXTDRV_CFLAGS += $(INC)
TARGET := hi_nvp6324

# compile linux
include $(MPP_PATH)/../drv/extdrv/Make.$(OSTYPE)
