#include <linux/platform_device.h>
#include <linux/slab.h>
#include <tx-isp.h>
struct tx_isp_my_device {
	/* the common parameters */
	struct tx_isp_subdev sd;
	spinlock_t slock;
	struct mutex mlock;
	int state;
        int my;
};

static int tx_isp_my_probe(struct platform_device *pdev)
{
	struct tx_isp_my_device *my_dev = NULL;
        int ret = 0;

        my_dev = (struct tx_isp_my_device *)kmalloc(sizeof(*my_dev), GFP_KERNEL);
        if(!my_dev){
                printk("Failed to allocate sensor device\n");
                ret = -1;
                goto exit;
        }
        memset(my_dev, 0, sizeof(*my_dev));
        my_dev->my = 2;

	platform_set_drvdata(pdev, my_dev);
	return 0;
exit:
	return ret;
}

static int __exit tx_isp_my_remove(struct platform_device *pdev)
{
	struct tx_isp_my_device *my_dev = platform_get_drvdata(pdev);
        kfree(my_dev);

	return 0;
}

struct platform_driver tx_isp_my_driver = {
	.probe = tx_isp_my_probe,
	.remove = __exit_p(tx_isp_my_remove),
	.driver = {
		.name = TX_ISP_MY_NAME,
		.owner = THIS_MODULE,
	},
};
