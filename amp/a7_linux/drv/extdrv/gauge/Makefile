CUR_ROOT    := $(shell pwd)
SDK_ROOT    ?= $(CUR_ROOT)/../../../../../
include $(SDK_ROOT)/build/base.mak

.PHONY : all clean

all:
ifeq ($(CONFIG_GAUGE_BQ27421),y)
	@if [ -d bq27421 ];then cd bq27421; make;fi
endif
ifeq ($(CONFIG_GAUGE_CW2015),y)
	@if [ -d cw2015 ];then cd cw2015; make;fi
endif
ifeq ($(CONFIG_GAUGE_MAX17201),y)
	@if [ -d MAX17201 ];then cd MAX17201; make;fi
endif
clean:
	@if [ -d bq27421 ];then cd bq27421; make clean;fi
	@if [ -d cw2015 ];then cd cw2015; make clean;fi
	@if [ -d MAX17201 ];then cd MAX17201; make clean;fi

