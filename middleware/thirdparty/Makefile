#==========================================================================
#                         export variable
#===============================================================================
LOCAL_DIR := $(shell pwd)
MIDDLEWARE_DIR := $(LOCAL_DIR)/..
include $(MIDDLEWARE_DIR)/base.mak

#===============================================================================
#                         local variable
#===============================================================================
CUNIT_DIR := c-unit


#===============================================================================
#                         rules
#===============================================================================
.PHONY : all clean install uninstall
#tiff compile rely on jpeg
exclude_softwares := tinyxml
exclude_softwares += c-unit
softwares := $(shell find . -maxdepth 1 -type d)
softwares := $(basename $(patsubst ./%,%,$(softwares)))
softwares := $(filter-out $(exclude_softwares), $(softwares))

all:
	if [ -d $(CUNIT_DIR) ]; then make -C $(CUNIT_DIR)||exit 1;fi
	for soft in $(softwares);do make -j32 -C $$soft||exit 1;done

clean:
	if [ -d $(CUNIT_DIR) ]; then make -C $(CUNIT_DIR) clean;fi
	for soft in $(softwares);do make -j32 -C $$soft clean;done

install:
	if [ -d $(CUNIT_DIR) ]; then make -C $(CUNIT_DIR) install||exit 1;fi
	for soft in $(softwares);do make -j32 -C $$soft install||exit 1;done

uninstall:
	if [ -d $(CUNIT_DIR) ]; then make -C $(CUNIT_DIR) clean;fi
	for soft in $(softwares);do make -j32 -C $$soft uninstall;done
