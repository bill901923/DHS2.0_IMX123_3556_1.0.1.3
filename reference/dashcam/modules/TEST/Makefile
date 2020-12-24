
OS_TYPE     := Linux
CUR_ROOT    := $(shell pwd)
PDT_ROOT    := $(CUR_ROOT)/../..
CODE_ROOT   := $(CUR_ROOT)/../../../..
include $(PDT_ROOT)/build/base.mak

SUBDIRS :=  


#++++++++++++++++++++++++++++++++++++++++++++++++++++
#	Compile Path
#++++++++++++++++++++++++++++++++++++++++++++++++++++
SRC_ROOT    := $(CUR_ROOT)/src
SRC_DIR     := $(SRC_ROOT)
OBJ_ROOT    := $(PDT_OUT_MODULES_OBJ)/$(CUR_PARENT_DIR_NAME)/$(OS_TYPE)
BIN_ROOT    := $(PDT_OUT_BIN)/$(CUR_DIR_NAME)

#++++++++++++++++++++++++++++++++++++++++++++++++++++
#	Source FileList
#++++++++++++++++++++++++++++++++++++++++++++++++++++
SRCS      := $(shell find $(SRC_DIR) -name '*.c')
SRCS      := $(sort $(SRCS))
ifneq ($(strip $(SRCS_OMIT)),)
SRCS      := $(filter-out $(SRCS_OMIT),$(SRCS))
endif

#++++++++++++++++++++++++++++++++++++++++++++++++++++
#	Object FileList
#++++++++++++++++++++++++++++++++++++++++++++++++++++
OBJS      := $(SRCS:$(SRC_ROOT)%.c=$(OBJ_ROOT)%.o)
OBJS      := $(sort $(OBJS))
OBJ_DIR   := $(sort $(dir $(OBJS)))
SRC_DEPS  := $(OBJS:%.o=%.d)

#++++++++++++++++++++++++++++++++++++++++++++++++++++
#	Module Specified Configure
#++++++++++++++++++++++++++++++++++++++++++++++++++++
MOD_INC_PATH := $(sort $(dir $(shell find $(CUR_ROOT) -name '*.h')))
VSS_INC_PATH += $(foreach dir, $(MOD_INC_PATH), -I$(dir))
ifeq ($(CFG_PQT_SUPPORT_ON),yes)
VSS_INC_PATH += -I$(PQT_STREAM_PATH)/slave/libhal_open/include
endif
VSS_INC_PATH += -I $(CUR_ROOT)/include
VSS_INC_PATH += -I $(CODE_ROOT)/amp/a7_liteos/mpp/include
VSS_INC_PATH += -I $(CODE_ROOT)/amp/a7_liteos/mpp/component/isp/user/3a/include
VSS_INC_PATH += -I $(CODE_ROOT)/amp/a7_liteos/mpp/sample/common
VSS_INC_PATH += -I $(CODE_ROOT)/amp/a7_linux/mpp/sample/common
VSS_INC_PATH += -I $(CODE_ROOT)/reference/hal/button/include
VSS_INC_PATH += -I $(CODE_ROOT)/reference/common/utils/include
VSS_INC_PATH += -I $(CODE_ROOT)/middleware/ndk/sample/arch/hi3559v200/comm
VSS_INC_PATH += -I $(CODE_ROOT)/middleware/ndk/sample/arch/common
VSS_INC_PATH += -I $(CODE_ROOT)/middleware/media_adpt/hi3559v200/ndk/code/mediaserver/utils
VSS_INC_PATH += -I $(CODE_ROOT)/middleware/ndk/code/include/inner
VSS_INC_PATH += -I $(CODE_ROOT)/middleware/ndk/code/include
VSS_INC_PATH += -I $(CODE_ROOT)/amp/a7_linux/mpp/include
VSS_INC_PATH += -I $(CODE_ROOT)/reference/dashcam/modules/scene/core/include/hi3559v200
VSS_INC_PATH += -I $(CODE_ROOT)/reference/dashcam/modules/scene/core/include
VSS_INC_PATH += -I $(CODE_ROOT)/reference/dashcam/modules/scene/core/clent
VSS_INC_PATH += -I $(CODE_ROOT)/reference/dashcam/modules/scene/core/server
VSS_INC_PATH += -I $(CODE_ROOT)/reference/thirdparty/libjpeg-turbo-1.5.3/out/include
VSS_INC_PATH += -I $(CODE_ROOT)/osdrv/opensource/kernel/linux-4.9.y/include

#VSS_INC_PATH += -I $(CODE_ROOT)/osdrv/platform/liteos/kernel/include
#VSS_INC_PATH += -I $(CODE_ROOT)/osdrv/platform/liteos/drivers/uart/include
#VSS_INC_PATH += -I $(CODE_ROOT)/osdrv/platform/liteos/compat/linux/include/linux
#VSS_INC_PATH += -I $(CODE_ROOT)/osdrv/platform/liteos/compat/linux/include
#VSS_INC_PATH += -I $(CODE_ROOT)/osdrv/platform/liteos/compat/linux/include/asm/
#VSS_INC_PATH += -I $(CODE_ROOT)/osdrv/platform/liteos/kernel/base/include
#VSS_INC_PATH += -I $(CODE_ROOT)/osdrv/platform/liteos/platform/bsp/common
#VSS_INC_PATH += -I $(CODE_ROOT)/osdrv/platform/liteos/platform/bsp/board/hi3559v200/include
#VSS_INC_PATH += -I $(CODE_ROOT)/osdrv/platform/liteos/platform/cpu/arm/arm-a/include
#VSS_INC_PATH += -I $(CODE_ROOT)/osdrv/platform/liteos/fs/include
MEDIAMODE_CNT = $(shell grep -nr "config_product_mediamode_*" $(PDT_INICFG_PATH)/config_cfgaccess_entry.ini |grep -v comm |wc -l)
VSS_MOD_CFLAGS := -DPDT_PARAM_MEDIAMODE_CNT=$(MEDIAMODE_CNT)
VSS_CFLAGS   += $(VSS_MOD_CFLAGS)
#++++++++++++++++++++++++++++++++++++++++++++++++++++
#	Create Directory
#++++++++++++++++++++++++++++++++++++++++++++++++++++
CreateResult :=
dummy := $(call CreateDir, $(OBJ_ROOT))
dummy += $(call CreateDir, $(BIN_ROOT))
dummy += $(foreach dir, $(OBJ_DIR), CreateResult += $(call CreateDir, $(dir)))
ifneq ($(strip CreateResult),)
    err = $(error $(CreateResult))
endif

#++++++++++++++++++++++++++++++++++++++++++++++++++++
#	Target
#++++++++++++++++++++++++++++++++++++++++++++++++++++
TARGET     := $(BIN_ROOT)/TEST_app
EXE        := $(TARGET)
MAP        := $(TARGET).map

#++++++++++++++++++++++++++++++++++++++++++++++++++++
#	Compile Command
#++++++++++++++++++++++++++++++++++++++++++++++++++++
COMPILE      = $(CC) $(VSS_CFLAGS) -Wall  -Werror -c "$<" -o "$@" $(VSS_INC_PATH)  -ffunction-sections -fdata-sections
COMPILEDEP   = $(CCDEP) -MM "$<"  $(VSS_CFLAGS) $(VSS_INC_PATH)
LINK         = $(CC) $(VSS_CFLAGS) -Wall

#++++++++++++++++++++++++++++++++++++++++++++++++++++
#	Link Libs
#++++++++++++++++++++++++++++++++++++++++++++++++++++
PDT_LIBS  :=	-lparam_host -lstatemng -l$(CFG_DASHCAM_TYPE) -lusbctrl -lnetctrl -lmedia_client

COMM_LIBS :=	-ldevmng -lrecordmng \
                -lstoragemng -lutils -lhilog -ltimedtask -ltimestamp -lmsg_client -lhicycle -lhiflash \
                -lhisnet -lpowercontrol -lfiletrans\
                -lsharefs -lscene_client -lhiosd_client -lsystem -lqueue


HAL_LIBS += -lhalgauge


MAPI_LIBS :=    -l$(CFG_CHIP_TYPE)_vcap_client -l$(CFG_CHIP_TYPE)_vproc_client \
                -l$(CFG_CHIP_TYPE)_venc_client -l$(CFG_CHIP_TYPE)_disp_client \
                -l$(CFG_CHIP_TYPE)_mapi_acap_client -l$(CFG_CHIP_TYPE)_mapi_aenc_client \
                -l$(CFG_CHIP_TYPE)_msg_client -l$(CFG_CHIP_TYPE)_sys_client \
                -l$(CFG_CHIP_TYPE)_mapi_ao_client \
                -l$(CFG_CHIP_TYPE)_log_client -lsecurec -ltde
ifeq ($(CFG_CHIP_TYPE),hi3559av100)
MAPI_LIBS +=    -lipcmsg_big-little -ldatafifo_big-little
else ifeq ($(CFG_CHIP_TYPE),hi3556av100)
MAPI_LIBS +=    -lipcmsg_a53_linux -ldatafifo_a53_linux
else ifeq ($(CFG_CHIP_TYPE),hi3519av100)
MAPI_LIBS +=    -lipcmsg_a53_linux -ldatafifo_a53_linux
else ifeq ($(CFG_CHIP_TYPE),hi3559v200)
MAPI_LIBS +=    -lipcmsg_a7_linux -ldatafifo_a7_linux
endif
MPP_LIBS  :=    -lmpi

ifneq ($(CONFIG_NONESCREEN), y)
MAPI_LIBS += -ltde
endif

#--------------------------------------------------------------------

VSS_LIB    := -L$(PDT_OUT_LIB_LINUX) -L$(PDT_OUT_MODULES_LIB)/$(OS_TYPE) -L$(REF_OUT_THIRD_LIB)
VSS_LIB    += $(PDT_LIBS)
VSS_LIB    += $(COMM_LIBS)
VSS_LIB    += $(HAL_LIBS)
VSS_LIB    += $(MWARE_LIBS)
VSS_LIB    += $(MAPI_LIBS)
VSS_LIB    += $(MPP_LIBS)
VSS_LIB    += -lpthread -lm -lrt -lstdc++ -ldl -lz
LINK_MAP = -Wl,-Map,$(MAP)

#--------------------------------------------------------------------

#++++++++++++++++++++++++++++++++++++++++++++++++++++
#	Compiler
#++++++++++++++++++++++++++++++++++++++++++++++++++++
.PHONY: all clean target
default: all

all:  target success

all : $(SUBDIRS)  $(SRC_DEPS) $(OBJS) target success


$(SUBDIRS):
	@echo "* SUBDIRS $(SUBDIRS)"
	@make -C $(SUBDIRS)||exit $?

success:
	@echo ""
	@echo -e "\033[31;32m `basename $(EXE)` Was SUCCESSFUL COMPILE \033[0m"
	@echo -e "\033[31;32m *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \033[0m"
	@echo "";echo ""

target: $(SRC_DEPS) $(OBJS)
	@echo "* VSS_LIB $(VSS_LIB)"
	@echo "* CREATE $(EXE)"
	@$(LINK) -o $(EXE) $(LINK_MAP) -Wl,--gc-sections -Wl,--start-group $(OBJS) $(VSS_LIB) -Wl,--end-group
#	@$(STRIP) $(EXE)
	@cp $(EXE) ~/TEST_app


$(SRC_DEPS) : $(OBJ_ROOT)/%.d : $(SRC_ROOT)/%.c
	@#echo "SRC_DEPS: " $(notdir $@)
	@set -e;$(COMPILEDEP) > $@.$$$$; \
	sed 's,.*\.o[ :]*,$(@:%.d=%.o) $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$

$(OBJS)     : $(OBJ_ROOT)/%.o : $(SRC_ROOT)/%.c
	@echo -e "\033[31;32m [$(CC)]  \033[0m `basename $<` \033[0m"
	@$(COMPILE)
	@echo -e "\033[31;32m       [OK] \033[0m";echo ""

clean:
	@for MOD in $(SUBDIRS);do\
		make clean -C $$MOD||exit $?;done
	@echo " "
	@echo "--------------------------------------------"
	@echo -e "\033[31;31m [DELETING Tmp files ......] \033[0m"
	@-rm -f $(SRC_DEPS)
	@if [ -n $(OBJ_ROOT) ];then rm -rf $(OBJ_ROOT);fi
	@echo -e "\033[31;31m [DELETING `basename $(TARGET)` ......] \033[0m"
	@rm -f $(TARGET)
	@echo "--------------------------------------------"
	@echo " "
