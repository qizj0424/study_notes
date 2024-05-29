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
	/*printk("isp device release!\n");*/
	return;
}

#if 1
/*********************************************************
 * tx_isp_widget_descriptor 普通设备驱动结构体
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
struct tx_isp_pad_descriptor tx_isp_fs_pads[] = {
       {
               .type = TX_ISP_PADTYPE_INPUT,
               .links_type = TX_ISP_PADLINK_DDR,
       },
       {
               .type = TX_ISP_PADTYPE_OUTPUT,
               .links_type = TX_ISP_PADLINK_DDR,
       },
       {
               .type = TX_ISP_PADTYPE_INPUT,
               .links_type = TX_ISP_PADLINK_DDR,
       },
       {
               .type = TX_ISP_PADTYPE_OUTPUT,
               .links_type = TX_ISP_PADLINK_DDR,
       },
	   {
               .type = TX_ISP_PADTYPE_INPUT,
               .links_type = TX_ISP_PADLINK_DDR,
       },
       {
               .type = TX_ISP_PADTYPE_OUTPUT,
               .links_type = TX_ISP_PADLINK_DDR,
       },
};

struct tx_isp_subdev_descriptor tx_isp_widget_fs = {
	.type = TX_ISP_TYPE_SUBDEV,
	.subtype = TX_ISP_SUBTYPE_CONTROLLER,
	.parentid = TX_ISP_SUBDEV_ID(0),
	.unitid = TX_ISP_WIDGET_ID(3),
	.clks_num = 0,
        .pads_num = ARRAY_SIZE(tx_isp_fs_pads),
        .pads = tx_isp_fs_pads,
};

struct platform_device tx_isp_fs_platform_device = {
	.name = TX_ISP_FS_NAME,
	.id = -1,
	.dev = {
		.dma_mask = &tx_isp_module_dma_mask,
		.coherent_dma_mask = 0xffffffff,
		.platform_data = &tx_isp_widget_fs,
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
struct tx_isp_pad_descriptor tx_isp_fs1_pads[] = {
        {
                .type = TX_ISP_PADTYPE_INPUT,
                .links_type = TX_ISP_PADLINK_DDR,
        },
        {
                .type = TX_ISP_PADTYPE_INPUT,
                .links_type = TX_ISP_PADLINK_DDR,
        },
        {
                .type = TX_ISP_PADTYPE_INPUT,
                .links_type = TX_ISP_PADLINK_DDR,
        },
};

struct tx_isp_subdev_descriptor tx_isp_widget_fs1 = {
	.type = TX_ISP_TYPE_SUBDEV,
	.subtype = TX_ISP_SUBTYPE_CONTROLLER,
	.parentid = TX_ISP_SUBDEV_ID(1),
	.unitid = TX_ISP_WIDGET_ID(4),
	.clks_num = 0,
        .pads_num = ARRAY_SIZE(tx_isp_fs1_pads),
        .pads = tx_isp_fs1_pads,
};

struct platform_device tx_isp_fs1_platform_device = {
	.name = TX_ISP_FS1_NAME,
	.id = -1,
	.dev = {
		.dma_mask = &tx_isp_module_dma_mask,
		.coherent_dma_mask = 0xffffffff,
		.platform_data = &tx_isp_widget_fs1,
		.release = tx_isp_release_device,
	},
	.resource = 0,
};
/*********************************************************
 * tx_isp_subdev_descriptor 是增加pad设备驱动
 * 通过pad可以和下级(out)设备通信
 *
 *
 *
*********************************************************/

struct tx_isp_pad_descriptor tx_isp_core_pads[] = {
	{
		.type = TX_ISP_PADTYPE_OUTPUT,
		.links_type = TX_ISP_PADLINK_DDR | TX_ISP_PADLINK_LFB,
	},
	{
		.type = TX_ISP_PADTYPE_OUTPUT,
		.links_type = TX_ISP_PADLINK_DDR | TX_ISP_PADLINK_LFB,
	},
	{
		.type = TX_ISP_PADTYPE_OUTPUT,
		.links_type = TX_ISP_PADLINK_DDR | TX_ISP_PADLINK_LFB,
	},
	{
		.type = TX_ISP_PADTYPE_UNDEFINE,
	},
};

struct tx_isp_subdev_descriptor tx_isp_subdev_ispcore = {
        .type = TX_ISP_TYPE_SUBDEV,
        .subtype = TX_ISP_SUBTYPE_PROCESSING_UNIT,
        .parentid = TX_ISP_HEADER_ID(0),
        .unitid = TX_ISP_SUBDEV_ID(0),
        /* .clks_num = ARRAY_SIZE(isp_core_clk_info), */
        /* .clks = isp_core_clk_info, */
        .pads_num = ARRAY_SIZE(tx_isp_core_pads),
        .pads = tx_isp_core_pads,
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
	&tx_isp_fs_platform_device,
	&tx_isp_fs1_platform_device,
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
 *此处表示设备驱动，实现在对应的设备驱动.c
 *
 *
 *
 *
*********************************************************/
static struct platform_driver *isp_drivers[] = {
	&tx_isp_vic_driver,
	&tx_isp_core_driver,
	&tx_isp_fs_driver,
	&tx_isp_fs1_driver,
};

static unsigned int drivers_num = ARRAY_SIZE(isp_drivers);


/*********************************************************
 *
 * pad link 在一起
 * link 相关
 *
 *
*********************************************************/
static struct tx_isp_link_config link1[] = {
	{
		.src = {TX_ISP_CORE_NAME, TX_ISP_PADTYPE_OUTPUT, 0},
		.dst = {TX_ISP_FS_NAME, TX_ISP_PADTYPE_INPUT, 0},
		.flag = TX_ISP_PADLINK_DDR | TX_ISP_PADLINK_FS,
	},
	{
		.src = {TX_ISP_FS_NAME, TX_ISP_PADTYPE_OUTPUT, 0},
		.dst = {TX_ISP_FS1_NAME, TX_ISP_PADTYPE_INPUT, 0},
		.flag = TX_ISP_PADLINK_DDR | TX_ISP_PADLINK_FS,
	},

	{
		.src = {TX_ISP_CORE_NAME, TX_ISP_PADTYPE_OUTPUT, 1},
		.dst = {TX_ISP_FS_NAME, TX_ISP_PADTYPE_INPUT, 1},
		.flag = TX_ISP_PADLINK_DDR | TX_ISP_PADLINK_FS,
	},
	{
		.src = {TX_ISP_FS_NAME, TX_ISP_PADTYPE_OUTPUT, 1},
		.dst = {TX_ISP_FS1_NAME, TX_ISP_PADTYPE_INPUT, 1},
		.flag = TX_ISP_PADLINK_DDR | TX_ISP_PADLINK_FS,
	},

	{
		.src = {TX_ISP_CORE_NAME, TX_ISP_PADTYPE_OUTPUT, 2},
		.dst = {TX_ISP_FS_NAME, TX_ISP_PADTYPE_INPUT, 2},
		.flag = TX_ISP_PADLINK_DDR | TX_ISP_PADLINK_FS,
	},
	{
		.src = {TX_ISP_FS_NAME, TX_ISP_PADTYPE_OUTPUT, 2},
		.dst = {TX_ISP_FS1_NAME, TX_ISP_PADTYPE_INPUT, 2},
		.flag = TX_ISP_PADLINK_DDR | TX_ISP_PADLINK_FS,
	},
};

static struct tx_isp_link_configs configs[] = {
	{link1, ARRAY_SIZE(link1)},
};

//static int configs_num = ARRAY_SIZE(configs);

static struct tx_isp_subdev_pad* find_subdev_link_pad(struct tx_isp_module *module, struct link_pad_description *desc)
{
	int index = 0;
	struct tx_isp_module *submod = NULL;
	struct tx_isp_subdev_pad* pad = NULL;
	struct tx_isp_subdev *subdev = NULL;

	for(index = 0; index < TX_ISP_ENTITY_ENUM_MAX_DEPTH; index++){
		if(module->submods[index]){
			submod = module->submods[index];
			if(strcmp(submod->name, desc->name) == 0)
				break;
			else
				submod = NULL;
		}
	}
	if(submod){
		subdev = module_to_subdev(submod);
		if(desc->type == TX_ISP_PADTYPE_INPUT && desc->index < subdev->num_inpads)
			pad = &(subdev->inpads[desc->index]);
		else if(desc->type == TX_ISP_PADTYPE_OUTPUT && desc->index < subdev->num_outpads)
			pad = &(subdev->outpads[desc->index]);
		else
			printk("Can't find the matched pad!\n");
	}

	return pad;
}


static int subdev_video_destroy_link(struct tx_isp_subdev_link *link)
{
	struct tx_isp_subdev_pad *source = NULL;	/* Source pad */
	struct tx_isp_subdev_pad *sink = NULL;		/* Sink pad  */
	struct tx_isp_subdev_link *backlink = NULL;

	if(link->flag == TX_ISP_LINKFLAG_DYNAMIC)
		return 0;
#if 0
	if(link->source->state == TX_ISP_PADSTATE_STREAM || link->sink->state == TX_ISP_PADSTATE_STREAM){
		printk("Please stop active links firstly! %d\n", __LINE__);
		return -EPERM;
	}
#endif
	source = link->source;
	sink = link->sink;
	backlink = link->reverse;

	link->source = NULL;
	link->sink = NULL;
	link->reverse = NULL;
	link->flag = TX_ISP_LINKFLAG_DYNAMIC;
	source->state = TX_ISP_PADSTATE_FREE;

	if(backlink){
		backlink->source = NULL;
		backlink->sink = NULL;
		backlink->reverse = NULL;
		backlink->flag = TX_ISP_LINKFLAG_DYNAMIC;
	}
	if(sink)
		sink->state = TX_ISP_PADSTATE_FREE;

	return 0;
}

static int subdev_video_setup_link(struct tx_isp_subdev_pad *local, struct tx_isp_subdev_pad *remote, unsigned int flag)
{
	int ret = 0;
	/*struct tx_isp_subdev_link *link = NULL;*/
	/*struct tx_isp_subdev_link *backlink = NULL;*/

	/* check links_type of pads  */
	if(!((local->links_type & remote->links_type) & flag)){
		printk("The link type is mismatch!\n");
		return -EPERM;
	}

	/* check state of pads */
	if(local->state == TX_ISP_PADSTATE_STREAM || remote->state == TX_ISP_PADSTATE_STREAM){
		printk("Please stop active links firstly! %d\n", __LINE__);
		return -EPERM;
	}

	if(local->state == TX_ISP_PADSTATE_LINKED){
		if(local->link.sink != remote){
			ret = subdev_video_destroy_link(&local->link);
			if(ret)
				return ret;
		}
	}

	if(remote->state == TX_ISP_PADSTATE_LINKED){
		if(remote->link.sink != local){
			ret = subdev_video_destroy_link(&remote->link);
			if(ret)
				return ret;
		}
	}

	local->link.source = local;
	local->link.sink = remote;
	local->link.reverse = &remote->link;
	local->link.flag = flag | TX_ISP_LINKFLAG_ENABLED;
	local->state = TX_ISP_PADSTATE_LINKED;

	remote->link.source = remote;
	remote->link.sink = local;
	remote->link.reverse = &local->link;
	remote->link.flag = flag | TX_ISP_LINKFLAG_ENABLED;
	remote->state = TX_ISP_PADSTATE_LINKED;

	return 0;
}

static int tx_isp_video_link_setup(struct tx_isp_module *module)
{
	int ret = 0;
	struct tx_isp_device *ispdev = module_to_ispdev(module);
	struct tx_isp_subdev_pad *src = NULL;
	struct tx_isp_subdev_pad *dst = NULL;
	struct tx_isp_link_config *config = NULL;
	int index = 0;
	int length = 0;
	int link = 0;
        int vinum;

        for (vinum = 0; vinum < 1; vinum ++) {
                /* printk("## %s %d link = %d ##\n", __func__, __LINE__, link); */
                /* if(ispdev->active_link[vinum] == link) */
                        /* return 0; */

                config = configs[link].config;
                length = configs[link].length;
                for(index = 0; index < length; index++){
                        src = find_subdev_link_pad(module, &(config[index].src));
                        dst = find_subdev_link_pad(module, &(config[index].dst));
                        if(src && dst){
                                ret = subdev_video_setup_link(src, dst, config[index].flag);
                                if(ret && ret != -ENOIOCTLCMD)
                                        goto exit;
                        }
                }
                ispdev->active_link[vinum] = link;
        }

	return 0;
exit:
	return ret;
}

static int tx_isp_video_link_destroy(struct tx_isp_module *module)
{
	int ret = 0;
	struct tx_isp_device *ispdev = module_to_ispdev(module);
	struct tx_isp_subdev_pad *src = NULL;
	struct tx_isp_subdev_pad *dst = NULL;
	struct tx_isp_link_config *config = NULL;
	int index = 0;
	int length = 0;
        int vinum = 0;

        for (vinum = 0; vinum < 1; vinum++) {
                if(ispdev->active_link[vinum] < 0)
                        return 0;

                /*printk("## %s %d link = %d ##\n", __func__, __LINE__, ispdev->active_link);*/
                config = configs[ispdev->active_link[vinum]].config;
                length = configs[ispdev->active_link[vinum]].length;
                for(index = 0; index < length; index++){
                        src = find_subdev_link_pad(module, &(config[index].src));
                        dst = find_subdev_link_pad(module, &(config[index].dst));
                        if(src && dst){
                                ret = subdev_video_destroy_link(&src->link);
                                ret = subdev_video_destroy_link(&dst->link);
                                if(ret && ret != -ENOIOCTLCMD)
                                        goto exit;
                        }
                }
                ispdev->active_link[vinum] = -1;
        }

	return 0;
exit:
	return ret;
}


int tx_isp_send_event_to_remote(struct tx_isp_subdev_pad *pad, unsigned int cmd, void *data)
{
	int ret = 0;
        if(pad && pad->link.sink && pad->link.sink->event) {
		ret = pad->link.sink->event(pad->link.sink, cmd, data);
        } else {
		ret = -ENOIOCTLCMD;
        }
	return ret;
}

/**********************************   new  ********************************************/

#if 0
static int tx_isp_notify(struct tx_isp_module *this, unsigned int notification, void *data)
{
	int ret = 0;
	struct tx_isp_module *module = &globe_ispdev->module;
	struct tx_isp_module *submod = NULL;
	struct tx_isp_subdev *subdev = NULL;
	int index = 0;

	/* */
	for(index = 0; index < TX_ISP_ENTITY_ENUM_MAX_DEPTH; index++){
		submod = module->submods[index];
		if(submod){
			subdev = module_to_subdev(submod);
			if(NOTIFICATION_TYPE_OPS(notification) == NOTIFICATION_TYPE_CORE_OPS)
				ret = tx_isp_subdev_call(subdev, core, ioctl, notification, data);
			else if(NOTIFICATION_TYPE_OPS(notification) == NOTIFICATION_TYPE_SENSOR_OPS)
				ret = tx_isp_subdev_call(subdev, sensor, ioctl, notification, data);
			else
				ret = 0;
			if(ret && ret != -ENOIOCTLCMD)
				break;
		}

	}
	if(ret == -ENOIOCTLCMD)
		return 0;

	return ret;
}
#endif

int tx_isp_module_init(struct platform_device *pdev, struct tx_isp_module *module)
{
	int ret = 0;
	struct tx_isp_descriptor *desc = NULL;

	if(!module){
		printk("%s the parameters are invalid!\n", __func__);
		return -EINVAL;
	}
	desc = pdev ? pdev->dev.platform_data : NULL;
	if(desc)
		module->desc = *desc;
	module->parent = NULL;
	memset(module->submods, 0, sizeof(module->submods));
	module->name = pdev->name;
	module->ops = NULL;
	module->debug_ops = NULL;
	module->dev = &(pdev->dev);
	//module->notify = tx_isp_notify;

	return ret;
}

void tx_isp_module_deinit(struct tx_isp_module *module)
{
	if(module)
		memset(module, 0, sizeof(*module));
}

static int tx_isp_subdev_device_init(struct tx_isp_subdev *sd, struct tx_isp_subdev_descriptor *desc)
{
	int ret = 0;
	int index = 0;
	int count = 0;
	struct tx_isp_subdev_pad *outpads = NULL;
	struct tx_isp_subdev_pad *inpads = NULL;

	/* init pads */
	for(index = 0; index < desc->pads_num; index++){
		if(desc->pads[index].type == TX_ISP_PADTYPE_OUTPUT)
			sd->num_outpads++;
		else
			sd->num_inpads++;
	}

        /* printk("## %s %d pads = %d inpads = %d, outpads = %d ##\n",__func__,__LINE__, desc->pads_num, sd->num_inpads, sd->num_outpads); */
	if(sd->num_outpads){
		outpads = (struct tx_isp_subdev_pad *)private_kmalloc(sizeof(*outpads) * sd->num_outpads, GFP_KERNEL);
		if(!outpads){
			printk("Failed to malloc %s's outpads\n",sd->module.name);
			ret = -ENOMEM;
			goto failed_outpads;
		}
		memset(outpads, 0, sizeof(*outpads) * sd->num_outpads);
		count = 0;
		for(index = 0; index < desc->pads_num; index++){
			if(desc->pads[index].type == TX_ISP_PADTYPE_OUTPUT){
				outpads[count].sd = sd;
				outpads[count].index = count;
				outpads[count].type = desc->pads[index].type;
				outpads[count].links_type = desc->pads[index].links_type;
				outpads[count].state = TX_ISP_PADSTATE_FREE;
				outpads[count].link.flag = TX_ISP_LINKFLAG_DYNAMIC;
				count++;
			}
		}
	}
	if(sd->num_inpads){
		inpads = (struct tx_isp_subdev_pad *)private_kmalloc(sizeof(*inpads) * sd->num_inpads, GFP_KERNEL);
		if(!inpads){
			printk("Failed to malloc %s's inpads\n",sd->module.name);
			ret = -ENOMEM;
			goto failed_inpads;
		}
		memset(inpads, 0, sizeof(*inpads) * sd->num_inpads);
		count = 0;
		for(index = 0; index < desc->pads_num; index++){
			if(desc->pads[index].type == TX_ISP_PADTYPE_INPUT){
				inpads[count].sd = sd;
				inpads[count].index = count;
				inpads[count].type = desc->pads[index].type;
				inpads[count].links_type = desc->pads[index].links_type;
				inpads[count].state = TX_ISP_PADSTATE_FREE;
				inpads[count].link.flag = TX_ISP_LINKFLAG_DYNAMIC;
				count++;
			}
		}
	}
	sd->outpads = outpads;
	sd->inpads = inpads;

	return 0;

failed_inpads:
	private_kfree(outpads);
failed_outpads:

	return ret;
}

static inline int tx_isp_subdev_widget_init(struct tx_isp_subdev *sd, struct tx_isp_widget_descriptor *desc)
{
        return 0;
}



int tx_isp_subdev_init(struct platform_device *pdev, struct tx_isp_subdev *sd, struct tx_isp_subdev_ops *ops)
{
	int ret = 0;
	struct tx_isp_device_descriptor *desc = NULL;
	if(!pdev || !sd){
		printk("%s the parameters are invalid!\n", __func__);
		return -EINVAL;
	}
	/* subdev ops */
	sd->ops = ops;

	/*printk("## %s %d ##\n",__func__,__LINE__);*/
	ret = tx_isp_module_init(pdev, &sd->module);
	if(ret){
		printk("Failed to init isp module(%s)\n", pdev->name);
		goto exit;
	}

	desc = pdev->dev.platform_data;
	if(desc == NULL)
		goto done;

	/* init subdev */
	if(desc->type == TX_ISP_TYPE_SUBDEV){
		ret = tx_isp_subdev_device_init(sd, (void*)desc);
	}else if(desc->type == TX_ISP_TYPE_WIDGET){
		ret = tx_isp_subdev_widget_init(sd, (void*)desc);
	}else{
		printk("It's header!\n");
	}
	if(ret){
		printk("[%d] Failed to init subdev!\n", __LINE__);
		goto failed_init;
	}

done:
	return 0;
failed_init:
	tx_isp_module_deinit(&sd->module);
exit:
	return ret;
}
EXPORT_SYMBOL(tx_isp_subdev_init);

void tx_isp_subdev_deinit(struct tx_isp_subdev *sd)
{
	struct tx_isp_module *module = &sd->module;

	if(module->ops)
		misc_deregister(&module->miscdev);
	if(sd->outpads)
		private_kfree(sd->outpads);
	if(sd->inpads)
		private_kfree(sd->inpads);
	tx_isp_module_deinit(&sd->module);
	sd->ops = NULL;
}
EXPORT_SYMBOL(tx_isp_subdev_deinit);


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
#if 0
	struct miscdevice *dev = file->private_data;
	struct tx_isp_module *module = miscdev_to_module(dev);
	struct tx_isp_device *ispdev = module_to_ispdev(module);
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

        tx_isp_video_link_destroy(module);

	misc_deregister(&module->miscdev);
	tx_isp_unregister_platforms(ispdev->pdevs);
	private_kfree(ispdev);
        return 0;
}

int tx_isp_create_graph_and_nodes(struct tx_isp_device* ispdev)
{
	int ret = 0;
	int index = 0;
	struct tx_isp_platform *pdevs = ispdev->pdevs;
	struct tx_isp_module *module = NULL;
	struct tx_isp_module *submodule = NULL;
	struct tx_isp_module *header = &ispdev->module;

	/* First, find subdevs */
	for(index = 0; index < ispdev->pdev_num; index++){
		module = platform_get_drvdata(pdevs[index].dev);
		/*printk("%s %d module = %p\n", __func__, __LINE__,module);*/
		if(IS_ERR_OR_NULL(module)){
			printk("Can't find the module(%d) driver!\n", index);
			continue;
		}
		if(module->desc.type == TX_ISP_TYPE_SUBDEV){
			header->submods[TX_ISP_GET_ID(module->desc.unitid)] = module;
		}
	}
	/*printk("%s %d\n", __func__, __LINE__);*/
	/* Second, find widgets */
	for(index = 0; index < ispdev->pdev_num; index++){
		module = platform_get_drvdata(pdevs[index].dev);
		if(module->desc.type == TX_ISP_TYPE_WIDGET){
			/*printk("%s %d  subdev %d, widget %d\n", __func__, __LINE__, TX_ISP_GET_ID(module->desc.parentid), TX_ISP_GET_ID(module->desc.unitid));*/
			submodule = header->submods[TX_ISP_GET_ID(module->desc.parentid)];
			if(submodule == NULL){
				printk("the module(%d.%d) doesn't have parent!\n",module->desc.parentid, module->desc.unitid);
				ret = -EINVAL;
				break;
			}
			submodule->submods[TX_ISP_GET_ID(module->desc.unitid)] = module;
		}
	}

	/*printk("%s %d\n", __func__, __LINE__);*/
	/* Thrid, create misc device and debug node */
	for(index = 0; index < ispdev->pdev_num; index++){
		module = platform_get_drvdata(pdevs[index].dev);
		if(module->ops){
			module->miscdev.minor = MISC_DYNAMIC_MINOR;
			module->miscdev.name = module->name;
			module->miscdev.fops = module->ops;
			ret = misc_register(&module->miscdev);
			if (ret < 0) {
				ret = -ENOENT;
				printk("Failed to register tx-isp miscdev(%d.%d)!\n", module->desc.parentid, module->desc.unitid);
				goto failed_misc_register;
			}
		}
		/* if(module->debug_ops){ */
			/* proc_create_data(module->name, S_IRUGO, ispdev->proc, module->debug_ops, (void *)module); */
		/* } */
	}

	return 0;
failed_misc_register:
	while(--index >= 0){
		module = platform_get_drvdata(pdevs[index].dev);
		if(module->ops){
			misc_deregister(&module->miscdev);
		}
	}

	return ret;
}

static int tx_isp_probe(struct platform_device *pdev)
{
	struct tx_isp_device* ispdev = NULL;
	struct tx_isp_device_descriptor *desc = NULL;
	struct tx_isp_module *module = NULL;
        int ret = 0;

	ispdev = (struct tx_isp_device*)private_kmalloc(sizeof(*ispdev), GFP_KERNEL);
	if (!ispdev) {
		printk("Failed to allocate camera device\n");
                return 0;
	}
	memset(ispdev, 0, sizeof(*ispdev));

	/* register subdev */
	desc = pdev->dev.platform_data;
	ispdev->pdev_num = desc->entity_num;
	tx_isp_match_and_register_platforms(desc, ispdev->pdevs);

	module = &ispdev->module;

	module->miscdev.minor = MISC_DYNAMIC_MINOR;
	module->miscdev.name = "tx-isp";
	module->miscdev.fops = &tx_isp_fops;
        //创建/dev/tx-isp/节点
	ret = misc_register(&module->miscdev);
	if (ret < 0) {
		ret = -1;
		printk("Failed to register tx-isp miscdev!\n");
		goto failed_misc_register;
	}


	ispdev->version = TX_ISP_DRIVER_VERSION;
        //将ispdev设置pdev私有数据
	platform_set_drvdata(pdev, ispdev);
        //创建各个子设备节点
        tx_isp_create_graph_and_nodes(ispdev);



        tx_isp_video_link_setup(module);

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
