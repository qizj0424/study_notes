#ifndef __TX_ISP_H__
#define __TX_ISP_H__

#include <linux/miscdevice.h>
#include <tx-isp-pad.h>
#include <tx-isp-libfunc.h>

#define TX_ISP_DRIVER_VERSION "H20231120a-zjqi"

#define TX_ISP_TYPE_HEADER				0x00
#define TX_ISP_TYPE_SUBDEV				0x01
#define TX_ISP_TYPE_WIDGET				0x02

#define TX_ISP_HEADER_ID(n)	(TX_ISP_TYPE_HEADER<<4 | n)
#define TX_ISP_SUBDEV_ID(n)	(TX_ISP_TYPE_SUBDEV<<4 | n)
#define TX_ISP_WIDGET_ID(n)	(TX_ISP_TYPE_WIDGET<<4 | n)
#define TX_ISP_GET_ID(n)	((n) & 0xf)

/* Video Device Descriptor Subtypes */
#define TX_ISP_SUBTYPE_UNDEFINE					(0x00)
#define TX_ISP_SUBTYPE_INPUT_TERMINAL			(0x01)
#define TX_ISP_SUBTYPE_OUTPUT_TERMINAL			(0x02)
#define TX_ISP_SUBTYPE_PROCESSING_UNIT			(0x03)
#define TX_ISP_SUBTYPE_CONTROLLER				(0x04)
#define TX_ISP_SUBTYPE_SELECTOR_UNIT			(0x05)

#define TX_ISP_VIN_NAME "isp-w00"
#define TX_ISP_CSI_NAME "isp-w01"
#define TX_ISP_VIC_NAME "isp-w02"
#define TX_ISP_FS_NAME "isp-fs"
#define TX_ISP_FS1_NAME "isp-fs1"
#define TX_ISP_CORE_NAME "isp-m0"
#define TX_ISP_IVDC_NAME "isp-ivdc"
#define TX_ISP_LDC_NAME "isp-m1"
#define TX_ISP_NCU_NAME "isp-m2"
#define TX_ISP_MSCALER_NAME "isp-m3"
#define TX_ISP_FS_NAME "isp-fs"
#define TX_ISP_DEV_NAME "isp-device"
#define TX_ISP_IRQ_ID 	"isp-irq-id"


#define TX_ISP_PADTYPE_UNDEFINE			0x00
#define TX_ISP_PADTYPE_INPUT			0x01
#define TX_ISP_PADTYPE_OUTPUT			0x02
#define TX_ISP_PADSTATE_FREE			(0x2)
#define TX_ISP_PADSTATE_LINKED			(0x3)
#define TX_ISP_PADSTATE_STREAM			(0x4)
#define TX_ISP_PADLINK_DDR				(0x1<<4)
#define TX_ISP_PADLINK_LFB				(0x1<<5)
#define TX_ISP_PADLINK_FS				(0x1<<6)

/* Video Device Descriptor link types */
#define TX_ISP_LINKFLAG_DYNAMIC		(0x0)
#define TX_ISP_LINKFLAG_ENABLED		(0x1)
#define TX_ISP_LINKFLAG(v)			((v) & 0xf)


#define TX_ISP_NAME_LEN 16
#define TX_ISP_PADS_PER_SUBDEV 8
#define TX_ISP_LINKS_PER_PADS 4

#define TX_ISP_PLATFORM_MAX_NUM 16


struct tx_isp_widget_descriptor {
	unsigned char  type;
	unsigned char  subtype;
	unsigned char  parentid;
	unsigned char  unitid;
	unsigned char  clks_num;
	struct tx_isp_device_clk *clks;
};


struct tx_isp_platform {
	struct platform_device *dev;
	struct platform_driver *drv;
};

/* All TX descriptors have these 2 fields at the beginning */
struct tx_isp_descriptor {
	unsigned char  type;
	unsigned char  subtype;
	unsigned char  parentid;
	unsigned char  unitid;
};

#define TX_ISP_ENTITY_ENUM_MAX_DEPTH	16
struct tx_isp_module {
	struct tx_isp_descriptor desc;
	struct device *dev;
	const char *name;
	struct miscdevice miscdev;
	/*struct list_head list;*/

	/* the interface */
	struct file_operations *ops;

	/* the interface */
	struct file_operations *debug_ops;

	/* the list header of sub-modules */
	struct tx_isp_module *submods[TX_ISP_ENTITY_ENUM_MAX_DEPTH];
	/* the module's parent */
	void *parent;
	int (*notify)(struct tx_isp_module *module, unsigned int notification, void *data);
};

struct tx_isp_device {
	struct tx_isp_module module;
	unsigned int pdev_num;
	struct tx_isp_platform pdevs[TX_ISP_PLATFORM_MAX_NUM];

	char *version;
	struct mutex mlock;
	int active_link[3];
	spinlock_t slock;
};

struct tx_isp_subdev_descriptor {
	unsigned char  type;
	unsigned char  subtype;
	unsigned char  parentid;
	unsigned char  unitid;
	unsigned char  clks_num;
	struct tx_isp_device_clk *clks;
	unsigned char  pads_num;
	struct tx_isp_pad_descriptor *pads;
};

/* Video device entity Descriptor */
struct tx_isp_device_descriptor {
	unsigned char  type;
	unsigned char  subtype;
	unsigned char  parentid;
	unsigned char  unitid;
	unsigned char  entity_num;
	struct platform_device **entities;
};

struct tx_isp_subdev {
	struct tx_isp_module module;

	/* basic members */
	struct resource *res;
	void __iomem *base;
	unsigned int clk_num;
        struct tx_isp_subdev_ops *ops;


	/* expanded members */
	unsigned short num_outpads;			/* Number of sink pads */
	unsigned short num_inpads;			/* Number of source pads */

	struct tx_isp_subdev_pad *outpads;		/* OutPads array (num_pads elements) */
	struct tx_isp_subdev_pad *inpads;		/* InPads array (num_pads elements) */

	void *dev_priv;
	void *host_priv;
};

#define miscdev_to_module(mdev) (container_of(mdev, struct tx_isp_module, miscdev))
#define module_to_subdev(mod) (container_of(mod, struct tx_isp_subdev, module))
#define irqdev_to_subdev(dev) (container_of(dev, struct tx_isp_subdev, irqdev))
#define module_to_ispdev(mod) (container_of(mod, struct tx_isp_device, module))

int tx_isp_subdev_init(struct platform_device *pdev, struct tx_isp_subdev *sd, struct tx_isp_subdev_ops *ops);
void tx_isp_subdev_deinit(struct tx_isp_subdev *sd);

int tx_isp_send_event_to_remote(struct tx_isp_subdev_pad *pad, unsigned int cmd, void *data);

extern struct platform_driver tx_isp_core_driver;
extern struct platform_driver tx_isp_vic_driver;
extern struct platform_driver tx_isp_fs_driver;
extern struct platform_driver tx_isp_fs1_driver;
#endif /*__TX_ISP_H__*/
