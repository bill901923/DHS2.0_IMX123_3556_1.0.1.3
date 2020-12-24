
CUR_ROOT    := $(shell pwd)
PDT_ROOT    ?= $(CUR_ROOT)/../..
OS_TYPE     := HuaweiLite
include $(PDT_ROOT)/build/base.mak

MODULE_ROOT := $(CUR_ROOT)
MODULE_NAME := $(CUR_DIR_NAME)

SRC_ROOT    := $(MODULE_ROOT)
SRC_DIR     := $(SRC_ROOT)/src

OBJ_ROOT    := $(PDT_OUT_MODULES_OBJ)/$(MODULE_NAME)/$(OS_TYPE)
LIB_ROOT    := $(PDT_OUT_MODULES_LIB)/$(OS_TYPE)
LIB_TYPE    := static

ORIG_RES_DIR		:= $(CUR_ROOT)/data/original
TRGT_RES_DIR		:= $(CUR_ROOT)/data/target
ifeq ($(CONFIG_BOOT_LOGO), y)
BOOT_LOGO_BMP		:=$(ORIG_RES_DIR)/bootlogo.bmp
BOOT_LOGO_YUV		:=$(TRGT_RES_DIR)/bootlogo.yuv
endif
ifeq ($(CONFIG_BOOT_SOUND), y)
BOOT_SOUND_WAV		:=$(ORIG_RES_DIR)/bootsound.wav
BOOT_SOUND_PCM		:=$(TRGT_RES_DIR)/bootsound.pcm
BOOT_SOUND_SAMPRATE	:=$(subst ",,$(word 3, $(shell grep -w sample_rate $(PDT_INICFG_PATH)/config_product_media_common.ini | tail -1 )))
endif
MAIN_LOGO_SIZE		:=160
SUB_LOGO_SIZE		:=86
OSD_LOGO_BMP		:=$(ORIG_RES_DIR)/osdlogo.bmp
OSD_LOGO_BIN_MAIN 	:=$(TRGT_RES_DIR)/osdlogo_$(MAIN_LOGO_SIZE).bin
OSD_LOGO_BIN_SUB	:=$(TRGT_RES_DIR)/osdlogo_$(SUB_LOGO_SIZE).bin
BMP2OSD_TOOL_PATH	:=$(REF_ROOT)/out/tools/bin/bmp2osd/bmp2osd

dummy := $(call CreateDir, $(TRGT_RES_DIR))

include $(REF_ROOT)/build/modbuild.mak

bootlogo:
ifeq ($(CONFIG_BOOT_LOGO), y)
	@echo -e "\033[31;32m [PRODUCE] `basename $(BOOT_LOGO_YUV)` ... ... \033[0m";
	@if [ ! -f  "$(BOOT_LOGO_YUV)" ]; then\
		ffmpeg -i $(BOOT_LOGO_BMP) -pix_fmt nv21 -v quiet $(BOOT_LOGO_YUV);\
	fi
	@echo -e "\033[31;32m [PRODUCE] `basename $(BOOT_LOGO_YUV)` SUCCESS \033[0m";
endif

bootsound:
ifeq ($(CONFIG_BOOT_SOUND), y)
	@echo -e "\033[31;32m [PRODUCE] `basename $(BOOT_SOUND_PCM)` ... ... \033[0m";
	@if [ ! -f  "$(BOOT_SOUND_PCM)" ]; then\
		ffmpeg -i $(BOOT_SOUND_WAV) -f s16le -ar $(BOOT_SOUND_SAMPRATE) -ac 1 -acodec pcm_s16le -v quiet $(BOOT_SOUND_PCM);\
	fi
	@echo -e "\033[31;32m [PRODUCE] `basename $(BOOT_SOUND_PCM)` SUCCESS \033[0m";
endif

osdlogo:
	@echo -e "\033[31;32m [PRODUCE] `basename $(OSD_LOGO_BIN_MAIN)` ... ... \033[0m";
	@if [ ! -f  "$(OSD_LOGO_BIN_MAIN)" ]; then\
		ffmpeg -i $(OSD_LOGO_BMP) -s $(MAIN_LOGO_SIZE)x$(MAIN_LOGO_SIZE) -v quiet $(TRGT_RES_DIR)/osdlogo_$(MAIN_LOGO_SIZE).bmp;\
		$(BMP2OSD_TOOL_PATH) $(TRGT_RES_DIR)/osdlogo_$(MAIN_LOGO_SIZE).bmp $(OSD_LOGO_BIN_MAIN);\
		rm $(TRGT_RES_DIR)/osdlogo_$(MAIN_LOGO_SIZE).bmp;\
	fi
	@echo -e "\033[31;32m [PRODUCE] `basename $(OSD_LOGO_BIN_MAIN)` SUCCESS \033[0m";
	@echo -e "\033[31;32m [PRODUCE] `basename $(OSD_LOGO_BIN_SUB)` ... ... \033[0m";
	@if [ ! -f  "$(OSD_LOGO_BIN_SUB)" ]; then\
		ffmpeg -i $(OSD_LOGO_BMP) -s $(SUB_LOGO_SIZE)x$(SUB_LOGO_SIZE) -v quiet $(TRGT_RES_DIR)/osdlogo_$(SUB_LOGO_SIZE).bmp;\
		$(BMP2OSD_TOOL_PATH) $(TRGT_RES_DIR)/osdlogo_$(SUB_LOGO_SIZE).bmp $(OSD_LOGO_BIN_SUB);\
		rm $(TRGT_RES_DIR)/osdlogo_$(SUB_LOGO_SIZE).bmp;\
	fi
	@echo -e "\033[31;32m [PRODUCE] `basename $(OSD_LOGO_BIN_SUB)` SUCCESS \033[0m";

resimage: bootlogo bootsound osdlogo
	@-cp $(ORIG_RES_DIR)/HZK16 $(TRGT_RES_DIR)
	@$(CUR_ROOT)/script/res2img.sh $(TRGT_RES_DIR) $(CUR_ROOT)/script/reslist.txt $(PDT_OUT_BURN)/resImage
	@if [  -f  "$(OSDRV_PUB_PC_BIN_PATH)/hi_gzip" ]; then\
		pushd $(OSDRV_PUB_PC_BIN_PATH);./hi_gzip $(PDT_OUT_BURN)/resImage $(PDT_OUT_BURN)/resImage_zip;popd;\
	else if	 [  -f  "$(OSDRV_PUB_PC_BIN_PATH)/hi_gzip/bin/hi_gzip" ]; then\
		pushd $(OSDRV_PUB_PC_BIN_PATH)/hi_gzip/bin;./hi_gzip $(PDT_OUT_BURN)/resImage $(PDT_OUT_BURN)/resImage_zip;popd;\
		fi;\
	fi

resimage_clean:
	@-rm $(PDT_OUT_BURN)/resImage_zip
	@-rm $(PDT_OUT_BURN)/resImage
	@-rm -rf $(TRGT_RES_DIR)