#===============================================================================
#                         export variable
#===============================================================================
LOCAL_DIR := $(shell pwd)
MIDDLEWARE_DIR := $(LOCAL_DIR)/..
include $(MIDDLEWARE_DIR)/base.mak
#===============================================================================
#                         rules
#===============================================================================
.PHONY : all clean install uninstall

exclude_components := netate

ifeq ($(CFG_CHIP_TYPE),hi3516cv500)
exclude_components += liteplayer
endif

components := $(shell find . -maxdepth 1 -type d)
components := $(basename $(patsubst ./%,%,$(components)))
components := $(filter-out $(exclude_components), $(components))

all:
	for comp in $(components);do make -j32 -C $$comp||exit 1;done

clean: $(components)
	for comp in $(components);do make -j32 -C $$comp clean;done

install: $(components)
	for comp in $(components);do make -j32 -C $$comp install;done

uninstall: $(components)
	for comp in $(components);do make -j32 -C $$comp uninstall;done
