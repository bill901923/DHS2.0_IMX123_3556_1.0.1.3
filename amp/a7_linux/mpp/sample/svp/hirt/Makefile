include $(PWD)/../Makefile.param

FLAGS := -O3 -Wall -fPIC -fstack-protector -fstack-check -Wl,-z,relro,-z,now -fvisibility=hidden
export FLAGS

CPPFLAGS += -I$(REL_INC)
CPPFLAGS += -I$(PWD)/common/include
CPPFLAGS += -I$(PWD)/include
CPPFLAGS += $(FLAGS)
CPPFLAGS += $(LIBS_CFLAGS)
CPPFLAGS += -DON_BOARD
CPU_AFFINITY := "cpu_task_affinity:1 cpu_task_affinity:1 monitor_task_affinity:1"
CPPFLAGS += -DCPU_AFFINITY=\"$(CPU_AFFINITY)\"
DEPEND_SDK_LIBS := -lnnie -ldl -lpthread -live -lsecurec -lmpi

SRCS := $(wildcard src/*.c)
SRCS += $(wildcard common/src/*.c)
TARGET := sample_runtime_release

# target source
OBJS  := $(SRCS:%.c=%.o)

.PHONY : all proposal clean

all: proposal $(TARGET)

proposal:
	make -f Makefile.plugin.proposal
	make -f Makefile.plugin.proposal_without_permute
	@cp plugins/proposal/*.so . -f
	@cp plugins/proposal_without_permute/*.so . -f

$(TARGET):$(OBJS)
	@cp $(PWD)/src/*.modelgroup $(PWD)
	$(CC) $(LIBS_LD_CFLAGS) -o $@ $^ -L$(REL_LIB) -Wl,-Bstatic -lsvpruntime -Wl,-Bdynamic -lpthread -lm -lstdc++ -L$(REL_LIB) $(DEPEND_SDK_LIBS)

clean:
	@rm -f $(TARGET)
	@rm -f $(OBJS)
	@rm -rf *.so
	@rm -rf *.ppm
	@rm -rf $(PWD)/*.modelgroup
	make clean -f Makefile.plugin.proposal
	make clean -f Makefile.plugin.proposal_without_permute
