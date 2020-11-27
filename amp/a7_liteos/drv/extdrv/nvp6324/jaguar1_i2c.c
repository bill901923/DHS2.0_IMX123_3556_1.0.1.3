/********************************************************************************
 *
 *  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
 *  Module		: i2c.c
 *  Description	:
 *  Author		:
 *  Date         :
 *  Version		: Version 1.0
 *
 ********************************************************************************
 *  History      :
 *
 *
 ********************************************************************************/
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#ifndef __HuaweiLite__
#include <linux/i2c-dev.h>
#endif


#ifdef __HuaweiLite__
#include "i2c.h"
#include "sys/statfs.h"
#include "fcntl.h"

#define DEV_REG_WIDTH   1
#define MAX_SENDDATA_LEN 8
static int g_i2c_fd = -1;

static unsigned char devaddr = 0;

static unsigned char hi_liteos_i2c_read(int reg, unsigned char * val, unsigned int data_len)
{
    int ret = 0;
    static struct i2c_rdwr_ioctl_data rdwr;
    static struct i2c_msg msg[2];
    unsigned char buf[4];

    memset(buf, 0x0, 4);
    msg[0].addr = devaddr;
    msg[0].flags = 0;
    msg[0].len = 1;
    msg[0].buf = buf;

    msg[1].addr = devaddr;
    msg[1].flags = 0;
    msg[1].flags |= I2C_M_RD;
    msg[1].len = 1;
    msg[1].buf = buf;

    rdwr.msgs = &msg[0];
    rdwr.nmsgs = (unsigned int)2;

    if (DEV_REG_WIDTH == 2)
    {
        buf[0] = (reg >> 8) & 0xff;
        buf[1] = reg & 0xff;
    } else
        buf[0] = reg & 0xff;

    ret = ioctl(g_i2c_fd, I2C_RDWR, &rdwr);
    if (ret != 2) {
        printk("CMD_I2C_READ error!\n");
        ret = -1;
        goto err;
    }
    *val = buf[0];

    return 0;
err:
    return -1;
}


static unsigned char hi_liteos_i2c_write(int reg, unsigned char * val, unsigned char data_len)
{
    int ret = 0;
    unsigned char sendbuf[MAX_SENDDATA_LEN];

    if (DEV_REG_WIDTH == 2) {
        sendbuf[0] = reg & 0xff;
        sendbuf[1] = (reg >> 8) & 0xff;
        memcpy(&sendbuf[2], val, data_len);
    }
    else
    {
        sendbuf[0] = reg & 0xff;
        memcpy(&sendbuf[1], val, data_len);
    }

    ret = write(g_i2c_fd, sendbuf, DEV_REG_WIDTH + data_len);
    if(ret < 0)
    {
        printk("write value error!\n");
        goto err;
    }

    return LOS_OK;
err:
    return -1;

}

int hi_liteos_i2c_init(unsigned int i2c_num,unsigned int dev_addr)
{
    int ret = 0;
    char file_name[0x20];

    snprintf(file_name, sizeof(file_name), "/dev/i2c-%u", i2c_num);
    g_i2c_fd = open(file_name, O_RDWR);
    if (g_i2c_fd < 0) {
        printk("open %s fail!\n",file_name);
        return -OS_FAIL;
    }
    ret = ioctl(g_i2c_fd, I2C_SLAVE_FORCE ,(dev_addr & 0xff));
    if (ret) {
        printk("set i2c_slave fail!\n");
        goto closefile;
    }
    devaddr = dev_addr;
    printk("-->>open[%s]g_i2c_fd[%d] dev_addr[%x] !!\n",file_name,g_i2c_fd,dev_addr);
    return LOS_OK;

closefile:
    close(g_i2c_fd);
    g_i2c_fd = -1;
    return -1;
}

int hi_liteos_i2c_exit(void)
{
    close(g_i2c_fd);
    g_i2c_fd = -1;
    return LOS_OK;
}
#endif

extern struct i2c_client* jaguar1_client;

void __I2CWriteByte8(unsigned char chip_addr, unsigned char reg_addr, unsigned char value)
{

#ifdef __HuaweiLite__
    hi_liteos_i2c_write(reg_addr,&value,1);
#else

    unsigned char tmp_buf[8];
    int ret = 0;
    int idx = 0;
    int reg_addr_num = 1;
    int data_byte_num = 1;
    struct i2c_client* client = jaguar1_client;
    int u32Tries = 0;

    client->addr = (chip_addr >> 1);

    /* reg_addr config */
    if (reg_addr_num == 1)
    {
        tmp_buf[idx++] = reg_addr & 0xff;
    }
    else
    {
        tmp_buf[idx++] = 0;//(reg_addr >> 8) & 0xff;
        tmp_buf[idx++] = reg_addr & 0xff;
    }

    /* data config */
    if (data_byte_num == 1)
    {
        tmp_buf[idx++] = value & 0xff;
    }
    else
    {
        tmp_buf[idx++] = 0;//(value >> 8) & 0xff;
        tmp_buf[idx++] = value & 0xff;
    }
    while (1)
    {
        ret = hi_i2c_master_send(client, tmp_buf, idx);
        if (ret == idx)
        {
            break;
        }
        else if ((ret == -EAGAIN) && (in_atomic() || irqs_disabled()))
        {
            u32Tries++;
            if (u32Tries > 5)
            {
                return;
            }
        }
        else
        {
            printk("[%s %d] i2c_master_send error, ret=%d. \n", __func__, __LINE__, ret);
            return;
        }
    }

    return;
 #endif
}

unsigned char __I2CReadByte8(unsigned char chip_addr, unsigned char reg_addr)
{
#ifdef __HuaweiLite__
    unsigned char value = 0;
    hi_liteos_i2c_read(reg_addr, &value, 1);
    return value;
#else
    unsigned char         ret_data = 0xFF;
    int                   ret;
    struct i2c_client*    client = jaguar1_client;
    static struct i2c_msg msg[2];
    unsigned char *       buffer;
    unsigned int          data_width = 1;

    buffer = kzalloc(data_width, GFP_ATOMIC);
    if(!buffer)
    {
        printk("NO memory.\n");
        return 0;
    }

    buffer[0]    = reg_addr;
    msg[0].addr  = client->addr;
    msg[0].flags = 0;
    msg[0].len   = 1;
    msg[0].buf   = buffer;

    msg[1].addr  = client->addr;
    msg[1].flags = client->flags | I2C_M_RD;
    msg[1].len   = data_width;
    msg[1].buf   = buffer;

    ret = hi_i2c_transfer(client->adapter, msg, 2);  // success : ret=2
    if (ret < 0)
    {
        printk("i2c read failed.\n");
    }
    else
    {
        printk("i2c read success buffer[0] = 0x%x.\n", buffer[0]);
        memcpy(&ret_data, buffer, data_width);
    }
    kfree(buffer);
    return ret_data;
#endif
}
