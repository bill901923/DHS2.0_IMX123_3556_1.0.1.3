
#ifndef __EMMC_RAW_H__
#define __EMMC_RAW_H__



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

HI_S32 hi_emmc_raw_init(HI_VOID);

HI_S32 hi_emmc_raw_destroy(HI_VOID);

HI_S32 hi_emmc_raw_read(HI_S32 fd, HI_U64 *startaddr, HI_U8 *buffer, HI_UL length,
                        HI_U64 openaddr, HI_U64 limit_leng, HI_S32 read_oob, HI_S32 skip_badblock);

HI_S32 hi_emmc_raw_write(HI_S32 fd, HI_U64 *startaddr, HI_U8 *buffer, HI_UL length,
                         HI_U64 openaddr, HI_U64 limit_leng, HI_S32 write_oob);

HI_S64 hi_emmc_raw_erase(HI_S32 fd, HI_U64 startaddr, HI_U64 length, HI_U64 openaddr, HI_U64 limit_leng);

HI_U64 hi_emmc_raw_get_start_addr(const HI_CHAR *dev_name, HI_UL blocksize, HI_S32 *value_valid);

HI_VOID hi_emmc_raw_get_info(HI_U64 *totalsize, HI_U32 *pagesize, HI_U32 *blocksize,
                             HI_U32 *oobsize, HI_U32 *blockshift);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __EMMC_RAW_H__ */

