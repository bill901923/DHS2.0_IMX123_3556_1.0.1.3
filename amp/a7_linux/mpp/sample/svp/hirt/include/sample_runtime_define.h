#ifndef __SAMPLE_RUNTIME_DEFINE_H
#define __SAMPLE_RUNTIME_DEFINE_H

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_ROI_NUM 300

#if defined ON_BOARD
#define RESOURCE_DIR            "./resource"
#define MODEL_ALEXNET           "runtime_alexnet_no_group_inst.wk"
#define MODEL_RFCN              "runtime_rfcn_resnet50_inst.wk"
#define MODEL_SSD               "runtime_ssd_inst.wk"
#define MODEL_GOTURN            "runtime_tracker_goturn_inst.wk"
#define IMAGE_ALEXNET           "ILSVRC2012_val_00000023_227x227.bgr"
#define IMAGE_RFCN              "horse_dog_car_person_600x800.bgr"
#define IMAGE_SSD               "dog_bike_car_300x300.bgr"
#define IMAGE_GOTURN            "%08llu.bgr"
#define CONFIG_DIR              "./"
#elif !defined USE_FUNC_SIM
#define RESOURCE_DIR            "../../data"
#define MODEL_ALEXNET           "classification/alexnet/inst/runtime_alexnet_no_group_inst.wk"
#define MODEL_RFCN              "detection/rfcn/resnet50/inst/runtime_rfcn_resnet50_inst.wk"
#define MODEL_SSD               "detection/ssd/inst/runtime_ssd_inst.wk"
#define MODEL_GOTURN            "tracker/goturn/inst/runtime_tracker_goturn_inst.wk"
#define IMAGE_ALEXNET           "classification/imagenet/test/ILSVRC2012_val_00000023_227x227.bgr"
#define IMAGE_RFCN              "detection/images/test/horse_dog_car_person_600x800.bgr"
#define IMAGE_SSD               "detection/images/test/dog_bike_car_300x300.bgr"
#define IMAGE_GOTURN            "tracker/images/jogging/%08llu.bgr"
#define CONFIG_DIR              "../src/"
#else
#define RESOURCE_DIR            "../../data"
#define MODEL_ALEXNET           "classification/alexnet/inst/runtime_alexnet_no_group_func.wk"
#define MODEL_RFCN              "detection/rfcn/resnet50/inst/runtime_rfcn_resnet50_func.wk"
#define MODEL_SSD               "detection/ssd/inst/runtime_ssd_func.wk"
#define MODEL_GOTURN            "tracker/goturn/inst/runtime_tracker_goturn_func.wk"
#define IMAGE_ALEXNET           "classification/imagenet/test/ILSVRC2012_val_00000023_227x227.bgr"
#define IMAGE_RFCN              "detection/images/test/horse_dog_car_person_600x800.bgr"
#define IMAGE_SSD               "detection/images/test/dog_bike_car_300x300.bgr"
#define IMAGE_GOTURN            "tracker/images/jogging/%08llu.bgr"
#define CONFIG_DIR              "../src/"
#endif

#define MODEL_ALEXNET_NAME      RESOURCE_DIR"/"MODEL_ALEXNET
#define MODEL_RFCN_NAME         RESOURCE_DIR"/"MODEL_RFCN
#define MODEL_SSD_NAME          RESOURCE_DIR"/"MODEL_SSD
#define MODEL_GOTURN_NAME       RESOURCE_DIR"/"MODEL_GOTURN

#define IMAGE_ALEXNET_NAME      RESOURCE_DIR"/"IMAGE_ALEXNET
#define IMAGE_RFCN_NAME         RESOURCE_DIR"/"IMAGE_RFCN
#define IMAGE_SSD_NAME          RESOURCE_DIR"/"IMAGE_SSD
#define IMAGE_GOTURN_NAME       RESOURCE_DIR"/"IMAGE_GOTURN
#ifdef __cplusplus
}
#endif

#endif
