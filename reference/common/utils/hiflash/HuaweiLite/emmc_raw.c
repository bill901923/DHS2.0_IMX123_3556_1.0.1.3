#include "mmc/mmc_os_adapt.h"
#include "mmc/block.h"
#include "hi_appcomm.h"
#include "../HuaweiLite/emmc_raw.h"

/*****************************************************************************/

HI_S32 hi_emmc_raw_init(HI_VOID)
{
    return 0;
}

HI_VOID hi_emmc_raw_get_info(HI_U64 *totalsize,
                             HI_U32 *pagesize,
                             HI_U32 *blocksize,
                             HI_U32 *oobsize,
                             HI_U32 *blockshift)
{
    extern struct disk_divide_info emmc;
    if (emmc.part_count > 0 && emmc.part[0].sector_start > 0) {
        *totalsize  = (HI_U64)emmc.part[0].sector_start * MMC_CARDBUS_BLOCK_SIZE;
    } else {
        *totalsize  = (HI_U64)emmc.sector_size * MMC_CARDBUS_BLOCK_SIZE;
    }
    *pagesize   = PAGE_SIZE;
    *blocksize  = MMC_CARDBUS_BLOCK_SIZE;
    *oobsize    = 0;
}

/*****************************************************************************/
/*
 * warning:
 *    1. startaddr should be alignment with pagesize
 */
HI_S32 hi_emmc_raw_read(HI_S32 fd,
                        HI_U64 *startaddr, /* this address maybe change when meet bad block */
                        HI_U8  *buffer,
                        HI_UL  length,    /* if HI_FLASH_RW_FLAG_WITH_OOB, include oob*/
                        HI_U64 openaddr,
                        HI_U64 limit_leng,
                        HI_S32 read_oob,
                        HI_S32 skip_badblock)
{
    HI_S32 start_sector = *startaddr / MMC_CARDBUS_BLOCK_SIZE;
    HI_S32 nsectors = length / MMC_CARDBUS_BLOCK_SIZE;
    if (length % MMC_CARDBUS_BLOCK_SIZE > 0) {
        nsectors++;
    }
    if (0 > emmc_raw_read((char *)buffer, start_sector, nsectors)) {
        MLOGE("\n error read emmc flash Addr:0x%08lx len:0x%lx\n", (HI_UL)*startaddr, length);
        return HI_FAILURE;
    }
    return length;
}

/*****************************************************************************/
/*
 * warning:
 *    1. offset and length should be alignment with blocksize
 */
HI_S64 hi_emmc_raw_erase(HI_S32 fd,
                         HI_U64 startaddr,
                         HI_U64 length,
                         HI_U64 openaddr,
                         HI_U64 limit_leng)
{
    MLOGW("emmc_raw_erase is not support\n");
    return length;
}

/*****************************************************************************/
/*
 * warning:
 *    1. startaddr should be alignment with pagesize
 */
HI_S32 hi_emmc_raw_write(HI_S32 fd,
                         HI_U64 *startaddr,
                         HI_U8 *buffer,
                         HI_UL length,
                         HI_U64 openaddr,
                         HI_U64 limit_leng,
                         HI_S32 write_oob)
{
    HI_S32 start_sector = *startaddr / MMC_CARDBUS_BLOCK_SIZE;
    HI_S32 nsectors = length / MMC_CARDBUS_BLOCK_SIZE;
    if (length % MMC_CARDBUS_BLOCK_SIZE > 0) {
        nsectors++;
    }
    if (0 > emmc_raw_write((char *)buffer, start_sector, nsectors)) {
        MLOGE("\n error write emmc flash Addr:0x%08lx len:0x%lx \n", (HI_UL)*startaddr, length);
        return HI_FAILURE;
    }
    return length;
}

/*****************************************************************************/

/*****************************************************************************/

HI_S32 hi_emmc_raw_destroy(HI_VOID)
{
    return 0;
}

