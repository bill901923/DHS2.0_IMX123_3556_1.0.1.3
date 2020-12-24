USE_SPECAWB=n
SDK_DIR=/home/xxxx/sdk/Hi3559V200_SDK_Vx.x.x.x

REL_DIR=$(SDK_DIR)/amp/a7_liteos/mpp
REL_LIB=$(REL_DIR)/lib

LITEOSTOPDIR=$(SDK_DIR)/osdrv/platform/liteos

CROSS=arm-himix100-linux-

HIARCH=hi3559v200

include $(LITEOSTOPDIR)/config.mk

CC = $(CROSS)gcc
AS  = $(CROSS)as
AR  = $(CROSS)ar
LD  = $(CROSS)ld
GPP = $(CROSS)g++
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump

PQT_LIB := $(shell pwd)/libcontrol.a
PQT_LIB += $(shell pwd)/libstream.a

SENSOR_LIBS := $(REL_LIB)/lib_hiae.a
SENSOR_LIBS += $(REL_LIB)/libisp.a
SENSOR_LIBS += $(REL_LIB)/lib_hidehaze.a
SENSOR_LIBS += $(REL_LIB)/lib_hidrc.a
SENSOR_LIBS += $(REL_LIB)/lib_hildci.a
SENSOR_LIBS += $(REL_LIB)/libsns_imx458.a
SENSOR_LIBS += $(REL_LIB)/libsns_imx307_2l.a
SENSOR_LIBS += $(REL_LIB)/libsns_imx307.a
SENSOR_LIBS += $(REL_LIB)/libsns_imx123_forcar.a
SENSOR_LIBS += $(REL_LIB)/libsns_os05a.a
SENSOR_LIBS += $(REL_LIB)/libsns_os08a10.a
SENSOR_LIBS += $(REL_LIB)/lib_hiawb.a

SDK_LIB_PATH := -L$(REL_LIB) -L$(REL_LIB)/extdrv
ifeq ($(USE_SPECAWB),y)
AWB_LIB=_hiawb_natura
else
AWB_LIB=_hiawb
endif
SDK_LIB := $(SDK_LIB_PATH) --start-group -lhi_osal -lmpi  -l$(HIARCH)_base -l$(HIARCH)_sys -lhi_user -lhdmi -l$(HIARCH)_isp -l$(HIARCH)_vi \
							-l$(HIARCH)_vo -l$(HIARCH)_vpss -l$(HIARCH)_vgs -l$(HIARCH)_gdc  -lhi_mipi_rx -lhi_mipi_tx\
							-l$(HIARCH)_chnl -l$(HIARCH)_rc -l$(HIARCH)_rgn -l$(HIARCH)_vedu \
							-l$(HIARCH)_venc -l$(HIARCH)_h265e -l$(HIARCH)_jpege -l$(HIARCH)_h264e \
							-l$(HIARCH)_dis  -l$(HIARCH)_gyrodis -l$(HIARCH)_motionfusion -lmotionfusion -l$(HIARCH)_hdmi  \
							-l_hidehaze -l_hidrc -l_hildci -l$(AWB_LIB) -l_hiae -lisp  -lhi_sensor_i2c\
							-laacdec -laacenc -lupvqe -ldnvqe -lVoiceEngine -l$(HIARCH)_ai -l$(HIARCH)_ao -l$(HIARCH)_aio -l$(HIARCH)_aenc -l$(HIARCH)_adec -l$(HIARCH)_acodec \
							-l$(HIARCH)_pm -lhi_piris  -lhi_ssp_st7789\
							-l$(HIARCH)_vdec -l$(HIARCH)_jpegd  -l$(HIARCH)_vfmw \
							-lhi_ssp_sony -lhi_sensor_spi -lhi_pwm \
							-lmotionsensor_chip -lmotionsensor_mng\
							--end-group
							
SDK_LIB += $(SENSOR_LIBS)

SDK_LIB += $(SDK_DIR)/amp/a7_liteos/hisyslink/ipcmsg/out/a7_liteos/libipcmsg_a7_liteos.a
SDK_LIB += $(SDK_DIR)/amp/a7_liteos/hisyslink/datafifo/out/a7_liteos/libdatafifo_a7_liteos.a

LITEOS_LIBDEPS = --start-group $(LITEOS_LIBDEP) --end-group $(LITEOS_TABLES_LDFLAGS)

LDFLAGS := $(LITEOS_LDFLAGS) --gc-sections

LIBS_CFLAGS :=  -fno-aggressive-loop-optimizations -ldl -ffunction-sections -fdata-sections
#MPP_CFLAGS := -Wall -Wno-error=implicit-function-declaration
MPP_CFLAGS := -Wall

CFLAGS :=
CFLAGS += $(LITEOS_CFLAGS)
CFLAGS += $(LIBS_CFLAGS)
CFLAGS += $(MPP_CFLAGS)
CFLAGS += -D__HuaweiLite__ -D__KERNEL__
ifeq ($(USE_SPECAWB),n)
CFLAGS += -D__NO_USE_SPECAWB__
endif
CFLAGS += -I$(REL_DIR)/include
CFLAGS += -I$(SDK_DIR)/amp/a7_liteos/osal/include

TARGET := pq_sample

# target source
SRCS  = $(shell pwd)/pq_sample.c
SRCS  += $(shell pwd)/pq_stream.c
SRCS  += $(shell pwd)/pq_control.c
SRCS  += $(REL_DIR)/init/sdk_init.c
SRCS  += $(REL_DIR)/init/sdk_exit.c
OBJS  := $(SRCS:%.c=%.o)

BIN := $(TARGET).bin
MAP := $(TARGET).map

.PHONY : clean all

all: $(BIN)

$(BIN):$(TARGET)
	@$(OBJCOPY) -O binary $(TARGET) $(BIN)

$(TARGET):$(OBJS)
	@$(LD) $(LDFLAGS) -Map=$(MAP) -o $(TARGET) $(OBJS) $(PQT_LIB) $(SDK_LIB) $(LITEOS_LIBDEPS) $(REL_LIB)/libsecurec.a
	@$(OBJDUMP) -d $(TARGET) > $(TARGET).asm

$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(TARGET) $(BIN) $(MAP) *.asm
	@rm -f $(OBJS)
