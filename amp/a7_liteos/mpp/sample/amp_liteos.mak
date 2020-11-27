SDK_LIB_PATH := -L$(REL_LIB) -L$(REL_LIB)/extdrv

SDK_LIB := $(SDK_LIB_PATH) --start-group -lhi_osal -lmpi -l$(ARCH_LIBNAME)_base -l$(ARCH_LIBNAME)_sys -lhi_user -lhdmi -l$(ARCH_LIBNAME)_isp -l$(ARCH_LIBNAME)_vi \
							-l$(ARCH_LIBNAME)_vo -l$(ARCH_LIBNAME)_vpss -l$(ARCH_LIBNAME)_vgs -l$(ARCH_LIBNAME)_gdc -lhi_mipi_rx -lhi_mipi_tx\
							-l$(ARCH_LIBNAME)_chnl -l$(ARCH_LIBNAME)_rc -l$(ARCH_LIBNAME)_rgn -l$(ARCH_LIBNAME)_vedu \
							-l$(ARCH_LIBNAME)_venc -l$(ARCH_LIBNAME)_h265e -l$(ARCH_LIBNAME)_jpege -l$(ARCH_LIBNAME)_h264e  \
							-l$(ARCH_LIBNAME)_dis  -l$(ARCH_LIBNAME)_hdmi\
							-l_hidehaze -l_hidrc -l_hildci -l_hiawb -l_hiae -lisp -lsns_imx123_forcar -lhi_sensor_i2c -lhi_ssp_st7789\
							-laacdec -laacenc -lupvqe -ldnvqe -lVoiceEngine -l$(ARCH_LIBNAME)_ai -l$(ARCH_LIBNAME)_ao -l$(ARCH_LIBNAME)_aio -l$(ARCH_LIBNAME)_aenc -l$(ARCH_LIBNAME)_adec -l$(ARCH_LIBNAME)_acodec \
							-l$(ARCH_LIBNAME)_vdec -l$(ARCH_LIBNAME)_jpegd  -l$(ARCH_LIBNAME)_vfmw \
							-lhi_ssp_sony -lhi_sensor_spi -lhi_pwm -lhi_piris  \
							-lhi_serdes \
							--end-group
							# -lhi_mipi_tx

ifeq ($(CONFIG_HI_MOTIONFUSION_SUPPORT), y)
SDK_LIB += -l$(ARCH_LIBNAME)_motionfusion
SDK_LIB += -lmotionsensor_mng
SDK_LIB += -lmotionsensor_chip
SDK_LIB += -lmotionfusion
SDK_LIB += -l$(ARCH_LIBNAME)_gyrodis
endif

ifeq ($(CONFIG_HI_PM_SUPPORT),y)
SDK_LIB += -l$(ARCH_LIBNAME)_pm
endif

ifeq ($(CONFIG_HI_SVP_CNN), y)
SDK_LIB += -lnnie
SDK_LIB += -l$(ARCH_LIBNAME)_nnie
endif

ifeq ($(CONFIG_HI_SVP_MD), y)
SDK_LIB += -lmd
endif

ifeq ($(CONFIG_HI_SVP_IVE), y)
SDK_LIB += -live
SDK_LIB += -l$(ARCH_LIBNAME)_ive
endif

SDK_LIB += $(SENSOR_LIBS)
SDK_LIB += $(wildcard $(SDK_PATH)/hisyslink/ipcmsg/out/a7_liteos/*.a)
SDK_LIB += $(wildcard $(SDK_PATH)/hisyslink/datafifo/out/a7_liteos/*.a)

LITEOS_LIBDEPS = --start-group $(LITEOS_LIBDEP) --end-group $(LITEOS_TABLES_LDFLAGS)

LDFLAGS := $(LITEOS_LDFLAGS) --gc-sections

# target source
SRCS  := $(AMP_SERVER_SRCS)
SRCS  += $(REL_DIR)/init/sdk_init.c
SRCS  += $(REL_DIR)/init/sdk_exit.c
OBJS  := $(SRCS:%.c=%.o)
OBJS += $(COMM_OBJ)
OBJS += $(MEDIA_MSG_SERVER_OBJ)

CFLAGS += $(COMM_INC)
CFLAGS += $(MEDIA_MSG_INC)
CFLAGS += $(AMP_INC)
CFLAGS += -I$(OSAL_ROOT)/include

BIN := $(TARGET_PATH)/$(TARGET).bin
MAP := $(TARGET_PATH)/$(TARGET).map

.PHONY : clean all

all: $(BIN)

$(BIN):$(TARGET)
	@$(OBJCOPY) -O binary $(TARGET_PATH)/$(TARGET) $(BIN)

$(TARGET):$(OBJS)
	@$(LD) $(LDFLAGS) -Map=$(MAP) -o $(TARGET_PATH)/$(TARGET) $(OBJS) $(SDK_LIB) $(LITEOS_LIBDEPS) $(REL_LIB)/libsecurec.a
	@$(OBJDUMP) -d $(TARGET_PATH)/$(TARGET) > $(TARGET_PATH)/$(TARGET).asm

$(OBJS):%.o:%.c
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(TARGET_PATH)/$(TARGET) $(BIN) $(MAP) $(TARGET_PATH)/*.asm
	@rm -f $(OBJS)
