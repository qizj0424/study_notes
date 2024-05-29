#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <driver-demo.h>

struct tx_isp_frame_channel {
	struct miscdevice	misc;
	char name[TX_ISP_NAME_LEN];
	struct tx_isp_subdev_pad *inpad;
	struct tx_isp_subdev_pad *outpad;
	int index;

	int state;
	unsigned int out_frames;
	unsigned int losed_frames;
	void *priv;
};

struct tx_isp_fs_device {
	/* the common parameters */
	struct tx_isp_subdev sd;
        struct tx_isp_frame_channel *chans;
	spinlock_t slock;
	struct mutex mlock;
	int num_chans;
	int state;
        int fs;
};

static struct tx_isp_subdev_ops fs_subdev_ops = {
	//.internal = &fs_internal_ops,
};

static int frame_chan_in_event(struct tx_isp_subdev_pad *pad, unsigned int event, void *arg)
{
	struct tx_isp_subdev *sd = pad->sd;
	struct tx_isp_fs_device *fs_dev = container_of((sd), struct tx_isp_fs_device, sd);
        printk("-- [%s,%d] This fs in event--\n",__func__,__LINE__);

        printk("goto in pad !!!\n");
	tx_isp_send_event_to_remote(fs_dev->chans[0].inpad, 0, NULL);
        printk("goto out pad !!!\n");
	tx_isp_send_event_to_remote(fs_dev->chans[0].outpad, 0, NULL);

        return 0;
}

static int frame_chan_out_event(struct tx_isp_subdev_pad *pad, unsigned int event, void *arg)
{
        printk("-- [%s,%d] This fs out event--\n",__func__,__LINE__);
#if 0
	struct tx_isp_subdev *sd = pad->sd;
	struct tx_isp_fs_device *fs_dev = container_of((sd), struct tx_isp_fs_device, sd);
        printk("-- [%s,%d] This fs out event--\n",__func__,__LINE__);

        printk("goto in pad !!!\n");
	tx_isp_send_event_to_remote(fs_dev->chans[0].inpad, 0, NULL);
        printk("goto out pad !!!\n");
	tx_isp_send_event_to_remote(fs_dev->chans[0].outpad, 0, NULL);
#endif

        return 0;
}

static int tx_isp_fs_probe(struct platform_device *pdev)
{
	struct tx_isp_fs_device *fs_dev = NULL;
	struct tx_isp_subdev *sd = NULL;
	struct tx_isp_device_descriptor *desc = NULL;
        int in_numchans;
        int ret = 0;
        int i = 0;

        fs_dev = (struct tx_isp_fs_device *)private_kmalloc(sizeof(*fs_dev), GFP_KERNEL);
        if(!fs_dev){
                printk("Failed to allocate sensor device\n");
                ret = -1;
                goto exit;
        }
        memset(fs_dev, 0, sizeof(*fs_dev));
        fs_dev->fs = 2;

	desc = pdev->dev.platform_data;
	sd = &fs_dev->sd;
	ret = tx_isp_subdev_init(pdev, sd, &fs_subdev_ops);
	if(ret){
		printk("Failed to init isp module(%d.%d)\n", desc->parentid, desc->unitid);
		ret = -ENOMEM;
		goto failed_to_ispmodule;
	}


        fs_dev->num_chans = sd->num_outpads;
        in_numchans  = sd->num_inpads;
	if(fs_dev->num_chans){
		fs_dev->chans = private_kmalloc(sizeof(struct tx_isp_frame_channel)*fs_dev->num_chans, GFP_KERNEL);
		memset(fs_dev->chans, 0, sizeof(struct tx_isp_frame_channel)*fs_dev->num_chans);
                for(i = 0; i < fs_dev->num_chans; i++){
                        fs_dev->chans[i].index = sd->outpads[i].index;
                        fs_dev->chans[i].outpad = &(sd->outpads[i]);
                        sd->outpads[i].event = frame_chan_out_event;
                }
                for(i = 0;i < in_numchans;i++){
                        fs_dev->chans[i].inpad = &(sd->inpads[i]);
                        sd->inpads[i].event = frame_chan_in_event;
                }
	}


	platform_set_drvdata(pdev, fs_dev);
	return 0;
failed_to_ispmodule:
	private_kfree(fs_dev);
exit:
	return ret;
}

static int __exit tx_isp_fs_remove(struct platform_device *pdev)
{
	struct tx_isp_fs_device *fs_dev = platform_get_drvdata(pdev);
        struct tx_isp_subdev *sd = &fs_dev->sd;
        tx_isp_subdev_deinit(sd);
        private_kfree(fs_dev->chans);
        private_kfree(fs_dev);

	return 0;
}

struct platform_driver tx_isp_fs_driver = {
	.probe = tx_isp_fs_probe,
	.remove = __exit_p(tx_isp_fs_remove),
	.driver = {
		.name = TX_ISP_FS_NAME,
		.owner = THIS_MODULE,
	},
};
