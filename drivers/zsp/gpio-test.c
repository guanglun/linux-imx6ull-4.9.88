#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>


#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <asm/mach/map.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <asm/io.h>
#include <linux/device.h>

#include <linux/platform_device.h>




/*------------------字符设备内容----------------------*/
#define DEV_NAME            "gpio-test"
#define DEV_CNT                 (1)

//定义字符设备的设备号
static dev_t gpio_test_devno;
//定义字符设备结构体chr_dev
static struct cdev gpio_test_chr_dev;


struct class *class_gpio_test;	//保存创建的类
struct device *device;	    // 保存创建的设备
struct device_node	*gpio_test_device_node; //设备树节点

int gpio_test1;			   
int gpio_test2;			   
int gpio_test3;	

void zsp_gpio_test1_write(char w)
{
	gpio_direction_output(gpio_test1, w);    
}
EXPORT_SYMBOL_GPL(zsp_gpio_test1_write);

void zsp_gpio_test2_write(char w)
{
	gpio_direction_output(gpio_test2, w);    
}
EXPORT_SYMBOL_GPL(zsp_gpio_test2_write);

void zsp_gpio_test3_write(char w)
{
	gpio_direction_output(gpio_test3, w);    
}
EXPORT_SYMBOL_GPL(zsp_gpio_test3_write);

/*字符设备操作函数集，open函数*/
static int gpio_test_chr_dev_open(struct inode *inode, struct file *filp)
{
	printk("\n open form driver \n");
    return 0;
}

/*字符设备操作函数集，write函数*/
static ssize_t gpio_test_chr_dev_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{

	
	unsigned char write_data; //用于保存接收到的数据

	int error = copy_from_user(&write_data, buf, cnt);
	if(error < 0) {
		return -1;
	}

	if(write_data & 0x01)
	{
		gpio_direction_output(gpio_test1, 1);  
	}
	else
	{
		gpio_direction_output(gpio_test1, 0);    
	}

	if(write_data & 0x02)
	{
		gpio_direction_output(gpio_test2, 1);  
	}
	else
	{
		gpio_direction_output(gpio_test2, 0);   
	}

	if(write_data & 0x04)
	{
		gpio_direction_output(gpio_test3, 1);  
	}
	else
	{
		gpio_direction_output(gpio_test3, 0);   
	}

	return 0;
}


/*字符设备操作函数集*/
static struct file_operations  gpio_test_chr_dev_fops = 
{
	.owner = THIS_MODULE,
    .open = gpio_test_chr_dev_open,
	.write = gpio_test_chr_dev_write,
};



/*----------------平台驱动函数集-----------------*/
static int gpio_test_probe(struct platform_device *pdv)
{
	
	int ret = 0;  //用于保存申请设备号的结果
    
	printk(KERN_EMERG"[ zsp gpio-test ]  match successed  \n");

    /*获取设备树节点*/
    gpio_test_device_node = of_find_node_by_path("/gpio_test");
    if(gpio_test_device_node == NULL)
    {
        printk(KERN_EMERG "\t  get gpio_test failed!  \n");
    }

    gpio_test1 = of_get_named_gpio(gpio_test_device_node, "gpio_test1", 0);
    gpio_test2 = of_get_named_gpio(gpio_test_device_node, "gpio_test2", 0);
	gpio_test3 = of_get_named_gpio(gpio_test_device_node, "gpio_test3", 0);


    gpio_direction_output(gpio_test1, 0);
    gpio_direction_output(gpio_test2, 0);
	gpio_direction_output(gpio_test3, 0);


	/*---------------------注册 字符设备部分-----------------*/

	//第一步
    //采用动态分配的方式，获取设备编号，次设备号为0，
    //设备名称为gpio-test，可通过命令cat  /proc/devices查看
    //DEV_CNT为1，当前只申请一个设备编号
    ret = alloc_chrdev_region(&gpio_test_devno, 0, DEV_CNT, DEV_NAME);
    if(ret < 0){
        printk("fail to alloc gpio_test_devno\n");
        goto alloc_err;
    }
    //第二步
    //关联字符设备结构体cdev与文件操作结构体file_operations
	gpio_test_chr_dev.owner = THIS_MODULE;
    cdev_init(&gpio_test_chr_dev, &gpio_test_chr_dev_fops);
    //第三步
    //添加设备至cdev_map散列表中
    ret = cdev_add(&gpio_test_chr_dev, gpio_test_devno, DEV_CNT);
    if(ret < 0)
    {
        printk("fail to add cdev\n");
        goto add_err;
    }

	//第四步
	/*创建类 */
	class_gpio_test = class_create(THIS_MODULE, DEV_NAME);

	/*创建设备*/
	device = device_create(class_gpio_test, NULL, gpio_test_devno, NULL, DEV_NAME);

	return 0;

add_err:
    //添加设备失败时，需要注销设备号
    unregister_chrdev_region(gpio_test_devno, DEV_CNT);
	printk("\n error! \n");
alloc_err:

	return -1;

}




static const struct of_device_id gpio_test[] = {
{ .compatible = "zsp,gpio-test"},
  { /* sentinel */ }
};

/*定义平台设备结构体*/
struct platform_driver gpio_test_platform_driver = {
	.probe = gpio_test_probe,
	.driver = {
		.name = "gpio-test-platform",
		.owner = THIS_MODULE,
		.of_match_table = gpio_test,
	}
};



/*
*驱动初始化函数
*/
static int __init gpio_test_platform_driver_init(void)
{
	int DriverState;
	
	DriverState = platform_driver_register(&gpio_test_platform_driver);
	
	printk(KERN_EMERG "\tDriverState is %d\n",DriverState);
	return 0;
}


/*
*驱动注销函数
*/
static void __exit gpio_test_platform_driver_exit(void)
{
	printk(KERN_EMERG "gpio test exit!\n");
	
	platform_driver_unregister(&gpio_test_platform_driver);	
}

subsys_initcall(gpio_test_platform_driver_init);
// module_init(gpio_test_platform_driver_init);
// module_exit(gpio_test_platform_driver_exit);

MODULE_AUTHOR("zsp");
MODULE_DESCRIPTION("zsp gpio_test driver");
MODULE_LICENSE("GPL");

