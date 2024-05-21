#include <linux/platform_device.h>
#include <linux/slab.h>
#include <tx-isp.h>

struct tx_isp_core_device {
	/* the common parameters */
	struct tx_isp_subdev sd;
	spinlock_t slock;
	struct mutex mlock;
	int state;
        int core;
};

static int tx_isp_core_probe(struct platform_device *pdev)
{
	struct tx_isp_core_device *core_dev = NULL;
        int ret = 0;

        core_dev = (struct tx_isp_core_device *)kmalloc(sizeof(*core_dev), GFP_KERNEL);
        if(!core_dev){
                printk("Failed to allocate sensor device\n");
                ret = -1;
                goto exit;
        }
        memset(core_dev, 0, sizeof(*core_dev));
        core_dev->core = 0;

	platform_set_drvdata(pdev, core_dev);

	return 0;
exit:
	return ret;
}

static int __exit tx_isp_core_remove(struct platform_device *pdev)
{
	struct tx_isp_core_device *core_dev = platform_get_drvdata(pdev);
        kfree(core_dev);

	return 0;
}

struct platform_driver tx_isp_core_driver = {
	.probe = tx_isp_core_probe,
	.remove = __exit_p(tx_isp_core_remove),
	.driver = {
		.name = TX_ISP_CORE_NAME,
		.owner = THIS_MODULE,
	},
};


