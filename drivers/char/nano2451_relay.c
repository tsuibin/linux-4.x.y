/*
1.driver/char/Kconfig
menu "Character devices"
++
config NANO_RELAY
        tristate "nanoPi2451 relay device GPG10"
        depends on MACH_MINI2451
        help

2.drvier/char/Makefile
++
obj-$(CONFIG_NANO_RELAY)        += nano2451_relay.o

3.make modules

4.insmod nano2451_relay.ko
*/

/*
//==========driver test app==========
#include     <stdio.h>
#include     <stdlib.h>
#include     <unistd.h>
#include     <sys/types.h>
#include     <sys/stat.h>
#include     <fcntl.h>
#include     <errno.h>
#define DEV_FILE "/dev/2451_relay"
int main()
{
        int fd_dev=-1;
        int dat;
        printf("nanoPi driver Test\n");

        fd_dev = open(DEV_FILE,O_RDWR);
        if(fd_dev<0){
                printf("open device err\n");
                return 0;
        }
        while(1){
                printf("Plese input 0/1:");
                scanf("%d",&dat);
                ioctl(fd_dev,dat);
        }
        return 0;
}
*/


#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/list.h>

#include <linux/clk.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio-samsung.h>

#define DEVICE_NAME "2451_relay"

//nanopi2451
#define LGPIO S3C2410_GPG(10)           //模块GPIO脚

static long relay_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
        switch(cmd) {
        case 0:
                gpio_set_value(LGPIO, 0);  //输出低电平
                return 0;
        case 1:
                gpio_set_value(LGPIO, 1);  //输出高电平
                return 0;
        default:
                return -EINVAL;
        }
}


static struct file_operations dev_fops={
        unlocked_ioctl:relay_ioctl,
};

static struct miscdevice misc = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = DEVICE_NAME,
        .fops = &dev_fops,
};

static int __init my_relay_init(void)
{
        int ret;

        s3c_gpio_cfgpin(LGPIO, S3C_GPIO_SFN(1));//设置输出
        s3c_gpio_setpull(LGPIO, S3C_GPIO_PULL_DOWN);//设置下拉

        ret = misc_register(&misc);
        printk (DEVICE_NAME"\tinitialized\n");

        return ret;
}

static void __exit my_relay_exit(void)
{
        misc_deregister(&misc);
}

module_init(my_relay_init);
module_exit(my_relay_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("o2ee");
