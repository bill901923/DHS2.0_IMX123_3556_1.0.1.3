#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/io.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/freezer.h>
#include <linux/input-polldev.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <asm/dma.h>

#include "i2cdev.h"

/** The Number should be change, if I2C device not used*/
#if defined (HI3559V200)
#define I2C_NUM 4
#endif
#define I2C_NUM 4

#define MAX1720x_SLAVE_ADDRESS_1 0x36
#define MAX1720x_SLAVE_ADDRESS_2 0x0B

static struct i2c_board_info hi_info[2] = {
     {I2C_BOARD_INFO("i2c-MAX17201-1", MAX1720x_SLAVE_ADDRESS_1)},
     {I2C_BOARD_INFO("i2c-MAX17201-2", MAX1720x_SLAVE_ADDRESS_2)}
};

struct i2c_client *hi_client[2];


int hi_i2c_read(unsigned int reg_addr, unsigned int reg_addr_num, unsigned char *reg_data, unsigned int data_num)
{
    unsigned char tmp_buf0[4];
    //unsigned char tmp_buf1[4];
    int ret = 0;
   // int ret_data = 0xFF;
    int idx = 0;
    struct i2c_client client;
    struct i2c_msg msg[2];
    unsigned char   Max_port= (reg_addr>>8)&0x01;
    memcpy(&client, hi_client[Max_port], sizeof(struct i2c_client));

    msg[0].addr = (client.addr);
    msg[0].flags = client.flags & I2C_M_TEN;
    msg[0].len = reg_addr_num;
    msg[0].buf = tmp_buf0;

    /* reg_addr config */
        tmp_buf0[idx++] = reg_addr&0xff;

    msg[1].addr = (client.addr);
    msg[1].flags = client.flags & I2C_M_TEN;
    msg[1].flags |= I2C_M_RD;
    msg[1].len = data_num;
    msg[1].buf = reg_data;

    while (1)
    {
        ret = i2c_transfer(client.adapter, msg, 2);
        if ((ret == -EAGAIN) && (in_atomic() || irqs_disabled()))
        {
            continue;
        }
        else if (ret == 2)
        {
            break;
        }
        else
        {
            printk("[%s %d] i2c_transfer error, ret=%d. \n", __func__, __LINE__,
                ret);
            break;
        }
    }

    return 0;
}




int hi_i2c_write(unsigned int reg_addr,unsigned int reg_addr_num, unsigned int data, unsigned int data_byte_num)
{
    int ret = 0;
    int idx = 0;
    unsigned char tmp_buf[8];
    unsigned char Max_port= (reg_addr>>8)&0x01;

    /* reg_addr config */
        tmp_buf[idx++] = reg_addr&0xff;

    /* data config */
    if(data_byte_num == 1)
    {
        tmp_buf[idx++] = data;
    }
    else
    {
        tmp_buf[idx++] = data&0xff;
        tmp_buf[idx++] = (data >> 8)&0xff;
    }

    while (1)
    {
        ret = i2c_master_send(hi_client[Max_port], tmp_buf, idx);
        if (ret == idx)
        {
            break;
        }
        #ifndef __LITEOS__
        else if ((ret == -EAGAIN) && (in_atomic() || irqs_disabled()))
        #else
        else if (ret == -EAGAIN)
        #endif
        {
            continue;
        }
        else
        {
            printk("[%s %d] i2c_master_send error, ret=%d. \n", __func__, __LINE__,
                ret);
            return ret;
        }
    }
    return 0;
}



int i2cdev_write(char *buf, unsigned int count,unsigned int reg_addr_num, unsigned int data_byte_num,unsigned int  Max_port)
{
    int ret;
    struct i2c_client *client = hi_client[Max_port];

    if (reg_addr_num == 2)
        client->flags |= I2C_M_16BIT_REG;

    if (data_byte_num == 2)
        client->flags |= I2C_M_16BIT_DATA;

    ret = i2c_master_send(client, buf, count);

    return ret;
}

int i2cdev_read(char *buf, unsigned int count,unsigned int reg_addr_num, unsigned int data_byte_num,unsigned int  Max_port)
{
    int ret;

    struct i2c_client *client = hi_client[Max_port];

    if (reg_addr_num == 2)
        client->flags |= I2C_M_16BIT_REG;

    if (data_byte_num == 2)
        client->flags |= I2C_M_16BIT_DATA;

    ret = i2c_master_recv(client, buf, count);

    return ret;
}

int i2cdev_init(void)
{
    struct i2c_adapter *i2c_adap;
    i2c_adap = i2c_get_adapter(I2C_NUM);
    if(i2c_adap == NULL)
    {
        dev_err(NULL ,"i2c_get_adapter error()!\n");
        return -1;
    }
    hi_client[0] = i2c_new_device(i2c_adap, &hi_info[0]);
    if(hi_client == NULL)
    {
        dev_err(NULL, "i2c_new_device 0 error()!\n");
        return -1;
    }

    hi_client[1] = i2c_new_device(i2c_adap, &hi_info[1]);
    if(hi_client == NULL)
    {
        dev_err(NULL, "i2c_new_device 1 error()!\n");
        return -1;
    }
#ifdef use_dma

     ret = i2cdev_mfd_init();
     if(ret != 0)
     {
         printk("%s, %s, %d line\n", __FILE__, __func__, __LINE__);
         return ret;
     }
#else
    i2c_put_adapter(i2c_adap);
#endif

    return 0;
}

void i2cdev_exit(void)
{
    if(hi_client[0] != NULL){
        i2c_unregister_device(hi_client[0]);
    }
    if(hi_client[1] != NULL){
        i2c_unregister_device(hi_client[1]);
    }
#ifdef use_dma
    i2cdev_mfd_exit();
#endif
}


