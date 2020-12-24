/**
 * @file    hi_appcomm_errno.h
 * @brief   describe common error code.
 *
 * Copyright (c) 2017 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/1/10
 * @version   1.0

 */
#ifndef _HI_APPCOMM_ERRNO_H_
#define _HI_APPCOMM_ERRNO_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif  /* __cplusplus */
#endif  /* __cplusplus */


/*************************************************************************
  typedef
*************************************************************************/

typedef HI_S32                  HI_ERRNO;


/*************************************************************************
  common error code
*************************************************************************/


#define HI_ERRNO_COMMON_BASE    0
#define HI_ERRNO_COMMON_COUNT   256

#define HI_EUNKNOWN             (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 1)  /* ����ԭ��δ֪��ȷ���Ѿ������ǲ����ж������ԭ�� */
#define HI_EOTHER               (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 2)  /* ��������֪������ԭ�������Թ��� */
#define HI_EINTER               (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 3)  /* �ڲ����������ڲ��������Դ���ĳЩ�ڴ������CPU�Բ��I/O����ѧ����������ȵ� */
#define HI_EVERSION             (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 4)  /* �汾���� */
#define HI_EPAERM               (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 5)  /* ��֧�ֵĲ���/����/���ԣ����������汾����δ���İ汾����������Ʒ����֧�� */
#define HI_EINVAL               (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 6)  /* �������󣬰������������������ò����������ò�����һ�»��г�ͻ�����ʵ��ı�š�ͨ���š��豸�š���ָ�롢��ַ��������ȵ� */
#define HI_ENOINIT              (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 7)  /* û�г�ʼ����ĳЩ�����������Ƚ��г�ʼ������ܽ��У�����δ���� */
#define HI_ENOTREADY            (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 8)  /* û��׼���ã�ĳЩ���������ھ߱�һ����������ܽ��У�������Ҫ�Ļ�������ȷ������ȱ���������Դ */
#define HI_ENORES               (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 9)  /* û����Դ�����������ڴ�ʧ�ܡ�û�п��л�������û�п��ж˿ڡ�û�п���ͨ���� */
#define HI_EEXIST               (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 10) /* ��Դ�Ѵ��ڣ���������½�����Դ�Ѿ����ڣ������������� */
#define HI_ELOST                (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 11) /* ��Դ�����ڣ�������ĳ����Դ����ַ���Ự������ */
#define HI_ENOOP                (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 12) /* ��Դ���ɲ������������𻵲���ʹ�á�У�����δ����Ԥ�ڵ����á��豸�����ݵȵ� */
#define HI_EBUSY                (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 13) /* ��Դ��æµ�����类���� */
#define HI_EIDLE                (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 14) /* ��Դ������ */
#define HI_EFULL                (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 15) /* ����ĳ���������Ѿ������� */
#define HI_EEMPTY               (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 16) /* �գ�ĳ���������ǿյ� */
#define HI_EUNDERFLOW           (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 17) /* ���磬ĳ�������е������Ѿ��½�������ˮ��֮�� */
#define HI_EOVERFLOW            (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 18) /* ���磬ĳ�������е���������������ˮ��֮�� */
#define HI_EACCES               (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 19) /* Ȩ�޴�������û��Ȩ�ޡ��������ȵ� */
#define HI_EINTR                (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 20) /* ����δ��ɣ��Ѿ��ж� */
#define HI_ECONTINUE            (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 21) /* ����δ��ɣ����ڼ��� */
#define HI_EOVER                (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 22) /* ������ɣ���û�к����Ĳ������� */
#define HI_ERRNO_COMMON_BOTTOM  (HI_ERRNO)(HI_ERRNO_COMMON_BASE + 23) /* �Ѷ���Ĵ���Ÿ��� */

/*************************************************************************
  custom error code
*************************************************************************/

#define HI_ERRNO_BASE           (HI_ERRNO)(HI_ERRNO_COMMON_BASE + HI_ERRNO_COMMON_COUNT)
#define HI_EINITIALIZED         (HI_ERRNO)(HI_ERRNO_BASE + 1) /* �ظ���ʼ�� */
#define HI_ERRNO_CUSTOM_BOTTOM  (HI_ERRNO)(HI_ERRNO_BASE + 2) /* �Ѷ���Ĵ���Ÿ��� */

#ifdef __cplusplus
#if __cplusplus
}
#endif  /* __cplusplus */
#endif  /* __cplusplus */

#endif  /* _HI_APPCOMM_ERRNO_H_ */

