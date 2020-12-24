
SUBDIRS := param usbctrl ui statemng media netctrl scene resource init DHS TEST

.PHONY: all $(SUBDIRS) clean
all:$(SUBDIRS) resimage

$(SUBDIRS):
	@make -C $@||exit $?

resimage:
	@make resimage -C resource||exit $?

clean:
	@for MOD in $(SUBDIRS);do\
		make clean -C $$MOD||exit $?;done
	@make resimage_clean -C resource||exit $?