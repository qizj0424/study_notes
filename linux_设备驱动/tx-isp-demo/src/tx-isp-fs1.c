#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <tx-isp.h>

struct tx_isp_frame_channel {
	struct miscdevice	misc;
	char name[TX_ISP_NAME_LEN];
	struct tx_isp_subdev_pad *inpad;
	int index;

	int state;
	unsigned int out_frames;
	unsigned int losed_frames;
	void *priv;
};

struct tx_isp_fs1_device {
	/* the common parameters */
	struct tx_isp_subdev sd;
        struct tx_isp_frame_channel *chans;
	spinlock_t slock;
	struct mutex mlock;
	int num_chans;
	int state;
};


static int frame_chan_in_event(struct tx_isp_subdev_pad *pad, unsigned int event, void *arg)
{
	struct tx_isp_subdev *sd = pad->sd;
	struct tx_isp_fs1_device *fs1_dev = container_of((sd), struct tx_isp_fs1_device, sd);
        printk("-- [%s,%d] This fs1 event--\n",__func__,__LINE__);

        printk("goto in pad !!!\n");
	tx_isp_send_event_to_remote(fs1_dev->chans[0].inpad, 0, NULL);
        return 0;
}


static struct tx_isp_subdev_ops fs1_subdev_ops = {
	//.internal = &fs_internal_ops,
};

static int tx_isp_fs1_probe(struct platform_device *pdev)
{
	struct tx_isp_fs1_device *fs1_dev = NULL;
	struct tx_isp_subdev *sd = NULL;
	struct tx_isp_device_descriptor *desc = NULL;
        int ret = 0;
        int i = 0;

        fs1_dev = (struct tx_isp_fs1_device *)private_kmalloc(sizeof(*fs1_dev), GFP_KERNEL);
        if(!fs1_dev){
                printk("Failed to allocate sensor device\n");
                ret = -1;
                goto exit;
        }
        memset(fs1_dev, 0, sizeof(*fs1_dev));

	desc = pdev->dev.platform_data;
	sd = &fs1_dev->sd;
	ret = tx_isp_subdev_init(pdev, sd, &fs1_subdev_ops);
	if(ret){
		printk("Failed to init isp module(%d.%d)\n", desc->parentid, desc->unitid);
		ret = -ENOMEM;
		goto failed_to_ispmodule;
	}


        fs1_dev->num_chans = sd->num_inpads;
	if(fs1_dev->num_chans){
		fs1_dev->chans = private_kmalloc(sizeof(struct tx_isp_frame_channel)*fs1_dev->num_chans, GFP_KERNEL);
		memset(fs1_dev->chans, 0, sizeof(struct tx_isp_frame_channel)*fs1_dev->num_chans);
		for(i = 0; i < fs1_dev->num_chans; i++){
                        fs1_dev->chans[i].index = sd->inpads[i].index;

                        fs1_dev->chans[i].inpad = &(sd->inpads[i]);
                        sd->inpads[i].event = frame_chan_in_event;
		}
	}


	platform_set_drvdata(pdev, fs1_dev);

	return 0;
failed_to_ispmodule:
	private_kfree(fs1_dev);
exit:
	return ret;
}

static int __exit tx_isp_fs1_remove(struct platform_device *pdev)
{
	struct tx_isp_fs1_device *fs1_dev = platform_get_drvdata(pdev);
        private_kfree(fs1_dev);

	return 0;
}

struct platform_driver tx_isp_fs1_driver = {
	.probe = tx_isp_fs1_probe,
	.remove = __exit_p(tx_isp_fs1_remove),
	.driver = {
		.name = TX_ISP_FS1_NAME,
		.owner = THIS_MODULE,
	},
};
