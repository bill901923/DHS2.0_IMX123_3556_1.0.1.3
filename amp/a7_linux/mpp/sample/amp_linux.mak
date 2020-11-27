# target source
OBJS := $(AMP_CLIENT_SRCS:%.c=%.o)

CFLAGS += $(MEDIA_MSG_INC)
CFLAGS += $(AMP_INC)

MPI_LIBS += $(SDK_PATH)/hisyslink/ipcmsg/out/a7_linux/libipcmsg_a7_linux.a
MPI_LIBS += $(SDK_PATH)/hisyslink/datafifo/out/a7_linux/libdatafifo_a7_linux.a
ifneq ($(CONFIG_HI_SUBARCH),hi3559v200)
  MPI_LIBS +=$(AUDIO_LIBA)
endif
.PHONY : clean all

all: $(TARGET)

$(TARGET): $(MEDIA_MSG_CLIENT_OBJ) $(OBJS)
	@$(CC) $(CFLAGS) -lpthread -lm -o $(TARGET_PATH)/$@ $^ $(MPI_LIBS) $(REL_LIB)/libsecurec.a

clean:
	@rm -f $(TARGET_PATH)/$(TARGET)
	@rm -f $(OBJS)
	@rm -f $(MEDIA_MSG_CLIENT_OBJ)
