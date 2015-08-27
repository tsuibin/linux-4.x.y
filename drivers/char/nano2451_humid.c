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

#define DEVICE_NAME "2451_humid"

//nanopi2451
#define LGPIO S3C2410_GPG(11)   //模块GPIO脚

static long humid_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
        switch(cmd) {
        case 0:
                gpio_set_value(LGPIO, 0);
                return 0;
        case 1:
                gpio_set_value(LGPIO, 1);
                //printk (DEVICE_NAME": %d %d\n", arg, cmd);
                return 0;
        default:
                return -EINVAL;
        }
}


static int humid_read(struct file *file, char * buffer, size_t count, loff_t * ppos)
{
        unsigned tmp;
        unsigned long err;
        tmp = gpio_get_value(LGPIO);
        printk("==%d==\n",tmp);
        err = copy_to_user(buffer, &tmp, 1);
        return 1;
}


static struct file_operations dev_fops={
        unlocked_ioctl:humid_ioctl,
        read:humid_read,
};


static struct miscdevice misc = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = DEVICE_NAME,
        .fops = &dev_fops,
};

static int __init my_humid_init(void)
{
        int ret;

        s3c_gpio_cfgpin(LGPIO, S3C_GPIO_SFN(0));//设置输入
//      s3c_gpio_setpull(LGPIO, S3C_GPIO_PULL_DOWN);//设置下拉

        ret = misc_register(&misc);
        printk (DEVICE_NAME"\tinitialized\n");

        return ret;
}

static void __exit my_humid_exit(void)
{
        misc_deregister(&misc);
}

module_init(my_humid_init);
module_exit(my_humid_exit);
