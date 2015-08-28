#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <mach/hardware.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include <linux/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio-samsung.h>


#define DEVICE_NAME "infrared"
#define tp_MAJOR 233

#define INFRARED_DATA_PIN S3C2410_GPG(10)

void infrared_init(void)
{
	s3c_gpio_cfgpin(INFRARED_DATA_PIN, S3C_GPIO_SFN(0));//set pin to be input
	gpio_set_value(INFRARED_DATA_PIN, 0);//set pin to be unknown state
}

unsigned char infrared_read(void)
{
	if(gpio_get_value(INFRARED_DATA_PIN))
		return 1;
	else
		return 0;
}

static ssize_t s3c2451_infrared_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	infrared_init();
	buf[0] = infrared_read();
	return 0;
}

static struct file_operations s3c2451_infrared_fops = {
	.owner = THIS_MODULE,
	.read = s3c2451_infrared_read,
};

static struct cdev cdev_infrared;

static int __init s3c2451_infrared_init(void)
{
	int result;
	dev_t devno = MKDEV(tp_MAJOR,0);
	struct class *tem_class;
	
	result = register_chrdev_region(devno,1,DEVICE_NAME);
	
	if(result){
		printk(KERN_NOTICE "Error %d register infrared",result);
		return result;
	}
	
	cdev_init(&cdev_infrared,&s3c2451_infrared_fops);
	
	result = cdev_add(&cdev_infrared,devno,1);
	if(result){
		printk(KERN_NOTICE "Error %d adding infrared",result);
		return result;
	}
	
	tem_class = class_create(THIS_MODULE, "tem_class");
	device_create(tem_class, NULL, MKDEV(tp_MAJOR, 0), "infrared","infdev%d", 0);
	
	return 0;  
}

static void __exit s3c2451_infrared_exit(void)
{
	cdev_del(&cdev_infrared);
	unregister_chrdev_region(MKDEV(tp_MAJOR,0),1);
}


module_init(s3c2451_infrared_init);
module_exit(s3c2451_infrared_exit);

MODULE_AUTHOR("CaZool");
MODULE_LICENSE("GPL");


