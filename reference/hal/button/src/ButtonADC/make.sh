INC=" -I ../../include"
INC+=" -I ../../../../../reference/dashcam/modules/ui/carrecorder/include"
INC+=" -I ../../../../../reference/dashcam/modules/init/amp/liteos/src"
INC+=" -I ../../../../../amp/a7_liteos/drv/extdrv/motionsensor/motionsensor_chip/pub"
INC+=" -I ../../../../../reference/common/utils/include"
INC+=" -I ../../../../../reference/common/utils/hilog/include"
INC+=" -I ../../../../../amp/a7_linux/mpp/include"
INC+=" -I ../../../../../middleware/component/higv/higv/include"
INC+=" -I ../../../../../middleware/out/linux/include"
INC+=" -I ../../../../../reference/dashcam/modules/statemng/include"
INC+=" -I ../../../../../reference/common/storagemng/include"
INC+=" -I ../../../../../reference/dashcam/modules/param/core/include"
INC+=" -I ../../../../../reference/dashcam/modules/media/include"
INC+=" -I ../../../../../reference/common/include"
INC+=" -I ../../../../../reference/common/hiosd/include"
INC+=" -I ../../../../../amp/a7_liteos/mpp/include"
INC+=" -I ../../../../../middleware/ndk/code/mediaserver/adapt/aenc/include"
INC+=" -I ../../../../../reference/common/photomng/include"
INC+=" -I ../../../../../reference/common/recordmng/include"
INC+=" -I ../../../../../reference/common/filemng/include"
INC+=" -I ../../../../../reference/hal/adc/include"
INC+=" -I ../../../../../reference/hal/common/include"
INC+=" -I ../../../../../middleware/common/include"
INC+=" -I ../../../../../amp/a7_linux/hisyslink/ipcmsg/include"
echo $INC
arm-himix100-linux-gcc -c hal_ButtonADC.c $INC
arm-himix100-linux-ar rcs libhal_ButtonADC.a hal_ButtonADC.o 
#arm-himix100-linux-gcc -o ButtonADC hal_ButtonADC.o ../../../../../middleware/common/log/lib/libmwlog.a

