.PHONY : all clean

exclude_components := ttf2asc debug WIFI_conf WIFI_tool other_file

components := $(shell find . -maxdepth 1 -type d)
components := $(basename $(patsubst ./%,%,$(components)))
components := $(filter-out $(exclude_components), $(components))

all:
	for comp in $(components);do make -C $$comp||exit;done

clean: $(components)
	for comp in $(components);do make -C $$comp clean;done

