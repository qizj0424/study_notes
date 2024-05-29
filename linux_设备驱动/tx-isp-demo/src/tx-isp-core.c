#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <tx-isp.h>

struct isp_core_output_channel {
	int index;
	int state;
	struct tx_isp_subdev_pad *pad;
	void *priv;
};

struct tx_isp_core_device {
	/* the common parameters */
	struct tx_isp_subdev sd;
	struct isp_core_output_channel *chans;
	spinlock_t slock;
	struct mutex mlock;
	int state;
        int num_chans;
};


static struct tx_isp_subdev_ops core_subdev_ops = {
#if 0
	.internal = &ispcore_internal_ops,
	.core = &ispcore_subdev_core_ops,
	.video = &ispcore_subdev_video_ops,
	.sensor = &ispcore_sensor_ops,
#endif
};

static int ispcore_pad_event_handle(struct tx_isp_subdev_pad *pad, unsigned int event, void *data)
{
        printk("-- [%s,%d] This core event--\n",__func__,__LINE__);
        return 0;
}

static int isp_core_output_channel_init(struct tx_isp_core_device *core)
{
	struct tx_isp_subdev *sd = &core->sd;
	struct isp_core_output_channel *chans = NULL;
	struct isp_core_output_channel *chan = NULL;
	int ret = 0;
	int index = 0;

	core->num_chans = sd->num_outpads;
	chans = (struct isp_core_output_channel *)private_kmalloc(sizeof(*chans) * sd->num_outpads, GFP_KERNEL);
	if(!chans){
		printk("Failed to allocate sensor device\n");
		ret = -1;
		goto exit;
	}

	memset(chans, 0, sizeof(*chans) * sd->num_outpads);

	for(index = 0; index < core->num_chans; index++){
		chan = &chans[index];
		chan->index = index;
		chan->pad = &(sd->outpads[index]);
		if(sd->outpads[index].type == TX_ISP_PADTYPE_UNDEFINE){
			continue;
		}
		chan->priv = core;
		sd->outpads[index].event = ispcore_pad_event_handle;
		sd->outpads[index].priv = chan;
	}

	core->chans = chans;
	return 0;
exit:
	return ret;
}

static int isp_core_open(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t  isp_core_write(struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
        int ret;
	struct miscdevice *dev = file->private_data;
	struct tx_isp_module *module = miscdev_to_module(dev);
	struct tx_isp_subdev *sd = module_to_subdev(module);
	struct tx_isp_core_device *core_dev = sd->dev_priv;

	ret = tx_isp_send_event_to_remote(core_dev->chans[0].pad, 0, NULL);
	return size;
}

static struct file_operations isp_core_fops = {
	.open = isp_core_open,
        .write = isp_core_write,
	/* .release = isp_core_release, */
	/* .unlocked_ioctl = isp_core_unlocked_ioctl, */
};

static int tx_isp_core_probe(struct platform_device *pdev)
{
	struct tx_isp_core_device *core_dev = NULL;
	struct tx_isp_subdev *sd = NULL;
	struct tx_isp_device_descriptor *desc = NULL;
        int ret = 0;

        core_dev = (struct tx_isp_core_device *)private_kmalloc(sizeof(*core_dev), GFP_KERNEL);
        if(!core_dev){
                printk("Failed to allocate sensor device\n");
                ret = -1;
                goto exit;
        }
        memset(core_dev, 0, sizeof(*core_dev));

	desc = pdev->dev.platform_data;
	sd = &core_dev->sd;
	ret = tx_isp_subdev_init(pdev, sd, &core_subdev_ops);
	if(ret){
		printk("Failed to init isp module(%d.%d)\n", desc->parentid, desc->unitid);
                goto free;
	}

	ret = isp_core_output_channel_init(core_dev);

	platform_set_drvdata(pdev, core_dev);
        sd->module.ops = &isp_core_fops;
        sd->dev_priv = core_dev;


	return 0;
free:
        private_kfree(core_dev);
exit:
	return ret;
}

static int __exit tx_isp_core_remove(struct platform_device *pdev)
{
        struct tx_isp_core_device *core_dev = platform_get_drvdata(pdev);
        struct tx_isp_subdev *sd = &core_dev->sd;

        tx_isp_subdev_deinit(sd);
        private_kfree(core_dev->chans);
        private_kfree(core_dev);

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


