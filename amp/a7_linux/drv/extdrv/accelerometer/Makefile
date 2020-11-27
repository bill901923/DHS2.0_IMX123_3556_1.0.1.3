CUR_ROOT    := $(shell pwd)
SDK_ROOT    ?= $(CUR_ROOT)/../../../
include $(SDK_ROOT)/build/base.mak

.PHONY : all clean

all:
ifeq ($(CONFIG_GSENSOR_MC3416),y)
	@if [ -d mc3416 ];then cd mc3416; make;fi
endif
ifeq ($(CONFIG_GSENSOR_SC7A20),y)
	@if [ -d sc7a20 ];then cd sc7a20; make;fi
endif
clean:
	@if [ -d mc3416 ];then cd mc3416; make clean;fi
	@if [ -d sc7a20 ];then cd sc7a20; make clean;fi
