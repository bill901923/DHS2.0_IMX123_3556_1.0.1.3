CUR_ROOT    := $(shell pwd)
SDK_ROOT    ?= $(CUR_ROOT)/../../../../../
include $(SDK_ROOT)/build/base.mak

.PHONY : all clean

all:
	@if [ -d sdio_rtl8189ftv ];then cd sdio_rtl8189ftv; make;fi


clean:
	@if [ -d sdio_rtl8189ftv ];then cd sdio_rtl8189ftv; make clean;fi

