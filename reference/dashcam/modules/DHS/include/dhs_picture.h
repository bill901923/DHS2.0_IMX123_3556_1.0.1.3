#ifndef __DHS_PICTURE_H__
#define __DHS_PICTURE_H__

#ifdef __cplusplus
extern "C" {
#endif

int takePhotoOfOnline(int sfd);
int takePhotoOfFIFO(int sfd,int type);
int sendPhoto(int sfd);
int sendPhoto_len(void);
#ifdef __cplusplus
}
#endif
#endif //__DHS_PICTURE_H__