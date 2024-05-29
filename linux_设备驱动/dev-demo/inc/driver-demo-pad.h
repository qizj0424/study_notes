#ifndef __TX_ISP_PAD_H__
#define __TX_ISP_PAD_H__

#include <driver-demo.h>
/* Description of the connection between modules */
struct link_pad_description {
	char *name; 		// the module name
	unsigned char type;	// the pad type
	unsigned char index;	// the index in array of some pad type
};

struct tx_isp_link_config {
	struct link_pad_description src;
	struct link_pad_description dst;
	unsigned int flag;
};

struct tx_isp_link_configs {
	struct tx_isp_link_config *config;
	unsigned int length;
};

/* The description of module entity */
struct tx_isp_subdev_link {
	struct tx_isp_subdev_pad *source;	/* Source pad */
	struct tx_isp_subdev_pad *sink;		/* Sink pad  */
	struct tx_isp_subdev_link *reverse;	/* Link in the reverse direction */
	unsigned int flag;				/* Link flag (TX_ISP_LINKTYPE_*) */
	unsigned int state;				/* Link state (TX_ISP_MODULE_*) */
};

struct tx_isp_subdev_core_ops {
#if 0
	int (*g_chip_ident)(struct tx_isp_subdev *sd, struct tx_isp_chip_ident *chip);
	int (*init)(struct tx_isp_subdev *sd, int on);		// clk's, power's and init ops.
	int (*reset)(struct tx_isp_subdev *sd, int on);
	int (*g_register)(struct tx_isp_subdev *sd, struct tx_isp_dbg_register *reg);
	int (*g_register_list)(struct tx_isp_subdev *sd, struct tx_isp_dbg_register_list *reg);
	int (*g_register_all)(struct tx_isp_subdev *sd, struct tx_isp_dbg_register_list *reg);
	int (*s_register)(struct tx_isp_subdev *sd, const struct tx_isp_dbg_register *reg);
	int (*ioctl)(struct tx_isp_subdev *, unsigned int , void *);
#endif
};

struct tx_isp_subdev_ops {
	struct tx_isp_subdev_core_ops		*core;
#if 0
	struct tx_isp_subdev_video_ops		*video;
	struct tx_isp_subdev_pad_ops		*pad;
	struct tx_isp_subdev_sensor_ops		*sensor;
	struct tx_isp_subdev_internal_ops	*internal;
#endif
};

/* Video pad Descriptor */
struct tx_isp_pad_descriptor {
	unsigned char  type;
	unsigned char  links_type;
	/*unsigned char  links_type[TX_ISP_LINKS_PER_PADS];*/
};

struct tx_isp_subdev_pad {
	struct tx_isp_subdev *sd;	/* Subdev this pad belongs to */
	unsigned char index;			/* Pad index in the entity pads array */
	unsigned char type;			/* Pad type (TX_ISP_PADTYPE_*) */
	unsigned char links_type;			/* Pad link type (TX_ISP_PADLINK_*) */
	unsigned char state;				/* Pad state (TX_ISP_PADSTATE_*) */
	struct tx_isp_subdev_link link;	/* The active link */
	int (*event)(struct tx_isp_subdev_pad *, unsigned int event, void *data);
	void *priv;
};
#endif /*__TX_ISP_PAD_H__*/


