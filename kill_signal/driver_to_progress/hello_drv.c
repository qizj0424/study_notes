#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <linux/sched.h>
#include <linux/signal.h>

/* 1. 确定主设备号                                                                 */
static int major = 0;
int vpid = 0;
static struct class *hello_class;
static struct task_struct *sd_to_pid = NULL;

static ssize_t hello_drv_write (struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
        int err,num;
        siginfo_t info;
        info.si_signo = SIGUSR1;
        info.si_code = -1;//__SI_RT;

        err = copy_from_user(&num, (int *)buf, sizeof(int));

        if(sd_to_pid && (num != 0xFFFF)){
                printk("send data : %d to progress %d\n",num, vpid);
                info.si_int = num; //私有数据
                send_sig_info(SIGUSR1,&info,sd_to_pid);
        }

        if(!sd_to_pid){
                vpid = num;
                printk("progress id is %d\n",vpid);
                sd_to_pid = pid_task(find_vpid(vpid), PIDTYPE_PID);;
        }

        if(sd_to_pid && (num == 0xFFFF)) {
                printk("free driver data !\n");
                sd_to_pid = NULL;

        }
        return 0;
}

static int hello_drv_open (struct inode *node, struct file *file)
{
	return 0;
}

static int hello_drv_close (struct inode *node, struct file *file)
{
	return 0;
}

/* 2. 定义自己的file_operations结构体                                              */
static struct file_operations hello_drv = {
	.owner	 = THIS_MODULE,
	.open    = hello_drv_open,
	.write   = hello_drv_write,
	.release = hello_drv_close,
};

/* 4. 把file_operations结构体告诉内核：注册驱动程序                                */
/* 5. 谁来注册驱动程序啊？得有一个入口函数：安装驱动程序时，就会去调用这个入口函数 */
static int __init hello_init(void)
{
	int err;

	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	major = register_chrdev(0, "hello", &hello_drv);  /* /dev/hello */


	hello_class = class_create(THIS_MODULE, "hello_class");
	err = PTR_ERR(hello_class);
	if (IS_ERR(hello_class)) {
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "hello");
		return -1;
	}

	device_create(hello_class, NULL, MKDEV(major, 0), NULL, "hello"); /* /dev/hello */

	return 0;
}

/* 6. 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数           */
static void __exit hello_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	device_destroy(hello_class, MKDEV(major, 0));
	class_destroy(hello_class);
	unregister_chrdev(major, "hello");
}


/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
