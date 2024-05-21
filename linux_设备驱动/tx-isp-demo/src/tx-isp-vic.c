#include <linux/platform_device.h>
#include <linux/slab.h>
#include <tx-isp.h>
struct tx_isp_vic_device {
	/* the common parameters */
	struct tx_isp_subdev sd;
	spinlock_t slock;
	struct mutex mlock;
	int state;
        int vic;
};

static int tx_isp_vic_probe(struct platform_device *pdev)
{
	struct tx_isp_vic_device *vic_dev = NULL;
        int ret = 0;

        vic_dev = (struct tx_isp_vic_device *)kmalloc(sizeof(*vic_dev), GFP_KERNEL);
        if(!vic_dev){
                printk("Failed to allocate sensor device\n");
                ret = -1;
                goto exit;
        }
        memset(vic_dev, 0, sizeof(*vic_dev));
        vic_dev->vic = 1;

	platform_set_drvdata(pdev, vic_dev);
	return 0;
exit:
	return ret;
}

static int __exit tx_isp_vic_remove(struct platform_device *pdev)
{
	struct tx_isp_vic_device *vic_dev = platform_get_drvdata(pdev);
        kfree(vic_dev);

	return 0;
}

struct platform_driver tx_isp_vic_driver = {
	.probe = tx_isp_vic_probe,
	.remove = __exit_p(tx_isp_vic_remove),
	.driver = {
		.name = TX_ISP_VIC_NAME,
		.owner = THIS_MODULE,
	},
};
