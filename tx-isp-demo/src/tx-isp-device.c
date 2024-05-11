#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
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
#include <linux/slab.h>
#include <tx-isp.h>

static u64 tx_isp_module_dma_mask = ~(u64)0;

static void tx_isp_release_device(struct device *dev)
{
	/*ISP_INFO("isp device release!\n");*/
	return;
}

#if 1
/*********************************************************
 *
 *
 *
 *
 *
*********************************************************/
#if 0
struct tx_isp_widget_descriptor tx_isp_widget_vic = {
	.type = TX_ISP_TYPE_WIDGET,
	.subtype = TX_ISP_SUBTYPE_CONTROLLER,
	.parentid = TX_ISP_SUBDEV_ID(0),
	.unitid = TX_ISP_WIDGET_ID(2),
	.clks_num = 0,
};
#endif

struct platform_device tx_isp_vic_platform_device = {
	.name = TX_ISP_VIC_NAME,
	.id = -1,
	.dev = {
		.dma_mask = &tx_isp_module_dma_mask,
		.coherent_dma_mask = 0xffffffff,
		.platform_data = NULL,
		.release = tx_isp_release_device,
	},
	.resource = 0,
};
#endif

/*********************************************************
 *
 *
 *
 *
 *
*********************************************************/
#if 0
struct tx_isp_widget_descriptor tx_isp_widget_vic = {
	.type = TX_ISP_TYPE_WIDGET,
	.subtype = TX_ISP_SUBTYPE_CONTROLLER,
	.parentid = TX_ISP_SUBDEV_ID(0),
	.unitid = TX_ISP_WIDGET_ID(2),
	.clks_num = 0,
};
#endif

struct platform_device tx_isp_my_platform_device = {
	.name = TX_ISP_MY_NAME,
	.id = -1,
	.dev = {
		.dma_mask = &tx_isp_module_dma_mask,
		.coherent_dma_mask = 0xffffffff,
		.platform_data = NULL,
		.release = tx_isp_release_device,
	},
	.resource = 0,
};
/*********************************************************
 *
 *
 *
 *
 *
*********************************************************/

struct tx_isp_subdev_descriptor tx_isp_subdev_ispcore = {
        .type = TX_ISP_TYPE_SUBDEV,
        .subtype = TX_ISP_SUBTYPE_PROCESSING_UNIT,
        .parentid = TX_ISP_HEADER_ID(0),
        .unitid = TX_ISP_SUBDEV_ID(0),
        /* .clks_num = ARRAY_SIZE(isp_core_clk_info), */
        /* .clks = isp_core_clk_info, */
        /* .pads_num = ARRAY_SIZE(tx_isp_core_pads), */
        /* .pads = tx_isp_core_pads, */
};

struct platform_device tx_isp_core_platform_device = {
	.name = TX_ISP_CORE_NAME,
	.id = -1,
	.dev = {
		.dma_mask = &tx_isp_module_dma_mask,
		.coherent_dma_mask = 0xffffffff,
		.platform_data = &tx_isp_subdev_ispcore,
		.release = tx_isp_release_device,
	},
	.num_resources = 0,
};

/*********************************************************
 *此处表示设备结构体，如需添加设备按照顺序添加即可，
 *设备驱动程序按照此顺序执行prob move等函数
 *prob move等函数 入参也是下面结构体
 *
 *
*********************************************************/
struct platform_device *tx_isp_devices[] = {
	/* isp core and its widgets */
	&tx_isp_core_platform_device,
	&tx_isp_vic_platform_device,
	&tx_isp_my_platform_device,
};

/* It's the root device */
struct tx_isp_device_descriptor tx_video_device = {
	.type = TX_ISP_TYPE_HEADER,
	.subtype = TX_ISP_SUBTYPE_CONTROLLER,
	.parentid = -1,
	.unitid = TX_ISP_HEADER_ID(0),
	.entity_num = ARRAY_SIZE(tx_isp_devices),
	.entities = tx_isp_devices,
};

struct platform_device tx_isp_platform_device = {
	.name = "tx-isp",
	.id = -1,
	.dev = {
		.dma_mask = &tx_isp_module_dma_mask,
		.coherent_dma_mask = 0xffffffff,
		.platform_data = &tx_video_device,
		.release = tx_isp_release_device,
	},
	.num_resources = 0,
};

/*********************************************************
 *此处表示设备驱动，实现在对于的设备驱动.c
 *
 *
 *
 *
*********************************************************/
static struct platform_driver *isp_drivers[] = {
	&tx_isp_vic_driver,
	&tx_isp_core_driver,
	&tx_isp_my_driver,
};

static unsigned int drivers_num = ARRAY_SIZE(isp_drivers);

static void tx_isp_unregister_platforms(struct tx_isp_platform *pform)
{
	int index = 0;

	for(index = 0; index < TX_ISP_PLATFORM_MAX_NUM; index++){
		if(pform[index].drv){
			pform[index].drv->remove(pform[index].dev);
		}
		if(pform[index].dev){
			platform_device_unregister(pform[index].dev);
		}
	}

	return;
}

static int tx_isp_match_and_register_platforms(struct tx_isp_device_descriptor *desc, struct tx_isp_platform *pform)
{
	struct platform_device *dev;
	struct platform_driver *drv;
	int dev_index, drv_index;
	int ret = 0;

	for(dev_index = 0; dev_index < desc->entity_num; dev_index++){
		dev = desc->entities[dev_index];
		pform[dev_index].dev = dev;
		for(drv_index = 0; drv_index < drivers_num; drv_index++){
			drv = isp_drivers[drv_index];
			if(strcmp(dev->name, drv->driver.name) == 0){
				ret = platform_device_register(dev);
				if(ret){
					printk("Failed to register isp device(%d)\n", dev_index);
					pform[dev_index].dev = NULL;
					goto error;
				}
				if(drv->probe)
					drv->probe(dev);
				pform[dev_index].drv = drv;
				break;
			}
		}
	}

	return 0;
error:
	tx_isp_unregister_platforms(pform);
	return ret;
}

static int tx_isp_open(struct inode *inode, struct file *file)
{
	struct miscdevice *dev = file->private_data;
	struct tx_isp_module *module = miscdev_to_module(dev);
	struct tx_isp_device *ispdev = module_to_ispdev(module);
#if 0
	struct tx_isp_module *submod = NULL;
	struct tx_isp_subdev *subdev = NULL;
	int index = 0;
	int ret = 0;

	for(index = 0; index < TX_ISP_ENTITY_ENUM_MAX_DEPTH; index++){
		submod = module->submods[index];
		if(submod){
			subdev = module_to_subdev(submod);
			ret = tx_isp_subdev_call(subdev, internal, activate_module);
			if(ret && ret != -1)
				break;
		}
	}
#endif
        return 0;
}

static int tx_isp_release(struct inode *inode, struct file *file)
{
        return 0;
}

static long tx_isp_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct miscdevice *dev = file->private_data;
	struct tx_isp_module *module = miscdev_to_module(dev);
	struct tx_isp_device *ispdev = module_to_ispdev(module);
        return 0;
}

static struct file_operations tx_isp_fops = {
	.open = tx_isp_open,
	.release = tx_isp_release,
	.unlocked_ioctl = tx_isp_unlocked_ioctl,
};

static int __exit tx_isp_remove(struct platform_device *pdev)
{
	struct tx_isp_device* ispdev = platform_get_drvdata(pdev);
	struct tx_isp_module *module = &ispdev->module;

	misc_deregister(&module->miscdev);
	tx_isp_unregister_platforms(ispdev->pdevs);
	kfree(ispdev);
        return 0;
}

static int tx_isp_probe(struct platform_device *pdev)
{
	struct tx_isp_device* ispdev = NULL;
	struct tx_isp_device_descriptor *desc = NULL;
	struct tx_isp_module *module = NULL;
        int ret = 0;

	ispdev = (struct tx_isp_device*)kmalloc(sizeof(*ispdev), GFP_KERNEL);
	if (!ispdev) {
		printk("Failed to allocate camera device\n");
                return 0;
	}
	memset(ispdev, 0, sizeof(*ispdev));

	/* register subdev */
	desc = pdev->dev.platform_data;
	tx_isp_match_and_register_platforms(desc, ispdev->pdevs);

	module = &ispdev->module;

	module->miscdev.minor = MISC_DYNAMIC_MINOR;
	module->miscdev.name = "tx-isp";
	module->miscdev.fops = &tx_isp_fops;
	ret = misc_register(&module->miscdev);
	if (ret < 0) {
		ret = -1;
		printk("Failed to register tx-isp miscdev!\n");
		goto failed_misc_register;
	}


	ispdev->version = TX_ISP_DRIVER_VERSION;
	platform_set_drvdata(pdev, ispdev);

        return 0;
failed_misc_register:
	return ret;
}

static struct platform_driver tx_isp_driver = {
	.probe = tx_isp_probe,
	.remove = __exit_p(tx_isp_remove),
	.driver = {
		.name = "tx-isp",
		.owner = THIS_MODULE,
	},
};

static int __init hello_init(void)
{
	/* int err; */
        int ret;

        ret = platform_device_register(&tx_isp_platform_device);
	if(ret){
		printk("Failed to insmod isp driver!!!\n");
		return ret;
	}

	ret = platform_driver_register(&tx_isp_driver);
	if(ret){
		platform_device_unregister(&tx_isp_platform_device);
	}


	return 0;
}

static void __exit hello_exit(void)
{
	platform_driver_unregister(&tx_isp_driver);
	platform_device_unregister(&tx_isp_platform_device);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
