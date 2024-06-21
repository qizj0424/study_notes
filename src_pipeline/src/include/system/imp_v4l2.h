/*
 * Emu v4l2 header file.
 *
 * Copyright (C) 2015 Ingenic Semiconductor Co.,Ltd
 * Author: Niky <xianghui.shen@ingenic.com>
 */

#ifndef __EMU_V4L2_H__
#define __EMU_V4L2_H__

#include <linux/videodev2.h>
#include <linux/v4l2-mediabus.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>

#include <imp/imp_isp.h>
#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
 * @file
 * ISP模块头文件
 */

typedef struct {
	uint16_t maxwidth;
	uint16_t maxheight;
	uint16_t minwidth;
	uint16_t minheight;
} IMPV4L2Scalercap;

typedef struct {
	uint16_t outwidth;
	uint16_t outheight;
} IMPV4L2Scaler;


/* define common struct */
enum tx_isp_priv_ioctl_direction {
	TX_ISP_PRIVATE_IOCTL_SET,
	TX_ISP_PRIVATE_IOCTL_GET,
};

/* isp image tuning */
struct isp_image_tuning_default_ctrl {
	enum tx_isp_priv_ioctl_direction dir;
	struct v4l2_control control;
	uint32_t vinum;
};

/**
 * struct frame_image_format
 * @type:	enum v4l2_buf_type; type of the data stream
 */
struct frame_image_format {
	unsigned int type;
	struct v4l2_pix_format pix;

	/* crop */
	bool	crop_enable;
	unsigned int crop_top;
	unsigned int crop_left;
	unsigned int crop_width;
	unsigned int crop_height;

	/* scaler */
	bool	scaler_enable;
	unsigned int scaler_out_width;
	unsigned int scaler_out_height;

	unsigned int rate_bits;
	unsigned int rate_mask;

	/* crop front */
	bool    fcrop_enable;
	unsigned int fcrop_top;
	unsigned int fcrop_left;
	unsigned int fcrop_width;
	unsigned int fcrop_height;
};

enum tx_isp_module_link_id {
	ISP_LINK_ISP_LFB_NCU_MS,
	ISP_LINK_ISP_DDR_NCU_MS,
	ISP_LINK_ISP_LDC_NCU_MS,
};


/* isp core tuning */
enum isp_image_tuning_private_cmd_id {
	IMAGE_TUNING_CID_AWB_ATTR = V4L2_CID_PRIVATE_BASE,
	IMAGE_TUNING_CID_AWB_CWF_SHIFT,
	IMAGE_TUNING_CID_MWB_ATTR,
	IMAGE_TUNING_CID_WB_STAINFO,
	IMAGE_TUNING_CID_WB_ATTR,
	IMAGE_TUNING_CID_WB_STATIS_ATTR,
	IMAGE_TUNING_CID_AWB_WEIGHT,
	IMAGE_TUNING_CID_AWB_HIST,
	IMAGE_TUNING_CID_AWB_RGB_COEFFT_WB_ATTR,
	IMAGE_TUNING_CID_WB_STATIS_GOL_ATTR,
	IMAGE_TUNING_CID_AWB_START,
	IMAGE_TUNING_CID_AWB_ZONE,
	IMAGE_TUNING_CID_AWB_ALGO,
	IMAGE_TUNING_CID_AWB_CT,
	IMAGE_TUNING_CID_AWB_CLUSTER,
	IMAGE_TUNING_CID_AWB_CT_TREND,
	IMAGE_TUNING_CID_AE_ATTR = V4L2_CID_PRIVATE_BASE + 0x20,
	IMAGE_TUNING_CID_AE_STAINFO,
	IMAGE_TUNING_CID_AE_STRATEGY,
	IMAGE_TUNING_CID_AE_COMP,
	IMAGE_TUNING_CID_AE_ROI,
	IMAGE_TUNING_CID_EXPR_ATTR,
	IMAGE_TUNING_CID_ISP_EV_ATTR,
	IMAGE_TUNING_CID_GET_TOTAL_GAIN,
	IMAGE_TUNING_CID_MAX_AGAIN_ATTR,
	IMAGE_TUNING_CID_MAX_DGAIN_ATTR,
	IMAGE_TUNING_CID_HILIGHT_DEPRESS_STRENGTH,
	IMAGE_TUNING_CID_GAMMA_ATTR,
	IMAGE_TUNING_CID_SYSTEM_TAB,
	IMAGE_TUNING_CID_AE_WEIGHT,
	IMAGE_TUNING_CID_AE_HIST,
	IMAGE_TUNING_CID_AE_MIN,
	IMAGE_TUNING_CID_AE_ZONE,
	IMAGE_TUNING_CID_AE_HIST_ORIGIN,
	IMAGE_TUNING_CID_AE_IT_MAX,
	IMAGE_TUNING_CID_AE_LUMA,
	IMAGE_TUNING_CID_AE_FREEZE,
	IMAGE_TUNING_CID_AE_MANUAL_ATTR,
	IMAGE_TUNING_CID_AE_STATE,
	IMAGE_TUNING_CID_BACKLIGHT_COMP_STRENGTH,
	IMAGE_TUNING_CID_AE_TARGET_LIST,
    IMAGE_TUNING_CID_DEFOG_STRENGTH,
	IMAGE_TUNING_CID_AF_ATTR = V4L2_CID_PRIVATE_BASE + 0x20 * 2,
	IMAGE_TUNING_CID_AF_STAINFO,
	IMAGE_TUNING_CID_AF_HIST,
	IMAGE_TUNING_CID_AF_METRIC,
	IMAGE_TUNING_CID_AF_WEIGHT,
	IMAGE_TUNING_CID_SENSOR_ATTR_CONTROL,
	IMAGE_TUNING_CID_AF_ZONE,
	IMAGE_TUNING_CID_DYNAMIC_DP_ATTR = V4L2_CID_PRIVATE_BASE + 0x20 * 3,
	IMAGE_TUNING_CID_STATIC_DP_ATTR,
	IMAGE_TUNING_CID_DPC_RATIO,
	IMAGE_TUNING_CID_NOISE_PROFILE_ATTR = V4L2_CID_PRIVATE_BASE + 0x20 * 4,
	IMAGE_TUNING_CID_SINTER_ATTR ,
	IMAGE_TUNING_CID_TEMPER_STRENGTH,
	IMAGE_TUNING_CID_TEMPER_ATTR,
	IMAGE_TUNING_CID_GET_NCU_INFO,
	IMAGE_TUNING_CID_3DNS_RATIO,
	IMAGE_TUNING_CID_2DNS_RATIO,
	IMAGE_TUNING_CID_DRC_ATTR = V4L2_CID_PRIVATE_BASE + 0x20 * 5,
	IMAGE_TUNING_CID_WDR_ATTR,
	IMAGE_TUNING_CID_DRC_RATIO,
	IMAGE_TUNING_CID_ENABLE_DRC,
	IMAGE_TUNING_CID_ENABLE_DEFOG,
	IMAGE_TUNING_CID_BLC_ATTR,
	IMAGE_TUNING_CID_CSC_ATTR,
	IMAGE_TUNING_CID_SHARP_ATTR = V4L2_CID_PRIVATE_BASE + 0x20 * 6,
	IMAGE_TUNING_CID_DEMO_ATTR,
	IMAGE_TUNING_CID_FC_ATTR,
	IMAGE_TUNING_CID_CONTROL_FPS= V4L2_CID_PRIVATE_BASE + 0x20 * 7,
	IMAGE_TUNING_CID_DAY_OR_NIGHT,
	IMAGE_TUNING_CID_MODULE_CONTROL,
	IMAGE_TUNING_CID_FCROP_CONTROL,
	IMAGE_TUNING_CID_HV_FLIP,
	IMAGE_TUNING_CID_MASK_CONTROL,
	IMAGE_TUNING_CID_EV_START,
	IMAGE_TUNING_CID_ISP_CUST_MODE,
	IMAGE_TUNING_CID_AUTOZOOM_CONTROL,
	IMAGE_TUNING_CID_SCALER_LEVEL,
	IMAGE_TUNING_CID_WDR_OUTPUT_MODE,
	IMAGE_TUNING_CID_CCM_ATTR = V4L2_CID_PRIVATE_BASE + 0x20 * 8,
	IMAGE_TUNING_CID_BCSH_HUE,
	IMAGE_TUNING_CID_BCSH_SET_FIXED_CONTRAST,
	IMAGE_TUNING_CID_SHAD_ATTR = V4L2_CID_PRIVATE_BASE + 0x20 * 9,
	IMAGE_TUNING_CID_GE_ATTR = V4L2_CID_PRIVATE_BASE + 0x20 * 10,
	IMAGE_TUNING_CID_DIS_STAINFO = V4L2_CID_PRIVATE_BASE + 0x20 * 11,
	IMAGE_TUNING_CID_ISP_TABLE_ATTR,
	IMAGE_TUNING_CID_ISP_WAIT_FRAME_ATTR,

	IMAGE_TUNING_CID_CUSTOM_ANTI_FOG,
	IMAGE_TUNING_CID_CUSTOM_ISP_PROCESS, 		//isp process
	IMAGE_TUNING_CID_CUSTOM_ISP_FREEZE, 		//isp process
	IMAGE_TUNING_CID_CUSTOM_SHAD,			//lens shading
	IMAGE_TUNING_CID_CUSTOM_TEMPER_DNS,		//temper denoise
	IMAGE_TUNING_CID_CUSTOM_DRC,			//raw dynamic range compression
	IMAGE_TUNING_CID_CUSTOM_WDR,			//sharpen
	IMAGE_TUNING_CID_FLIKER_DETECT,

	IMAGE_TUNING_CID_DRAW_BLOCK_ATTR = V4L2_CID_PRIVATE_BASE + 0x20 * 12,
	IMAGE_TUNING_CID_OSD_ATTR,
	IMAGE_TUNING_CID_OSD_BLOCK_ATTR,
	IMAGE_TUNING_CID_MASK_BLOCK_ATTR,
	IMAGE_TUNING_CID_MODULE_RATIO,
	IMAGE_TUNING_CID_SWITCH_BIN,

        IMAGE_TUNING_CID_SENSOR_H_FLIP,
        IMAGE_TUNING_CID_SENSOR_V_FLIP,
};

struct isp_buf_info {
	uint32_t vinum;
    uint32_t paddr;
    uint32_t size;
};

struct tx_isp_initarg {
        int enable;
        int vinum;
};

#define VIDIOC_REGISTER_SENSOR		 _IOW('V', BASE_VIDIOC_PRIVATE + 1, IMPSensorInfo)
#define VIDIOC_RELEASE_SENSOR		 _IOW('V', BASE_VIDIOC_PRIVATE + 2, IMPSensorInfo)
#define VIDIOC_SET_FRAME_FORMAT		_IOWR('V', BASE_VIDIOC_PRIVATE + 3, struct frame_image_format)
#define VIDIOC_RESET_FRAME_FORMAT		_IOWR('V', BASE_VIDIOC_PRIVATE + 22, struct frame_image_format)
#define VIDIOC_GET_FRAME_FORMAT		_IOR('V', BASE_VIDIOC_PRIVATE + 4, struct frame_image_format)
#define VIDIOC_DEFAULT_CMD_SET_BANKS	 _IOW('V', BASE_VIDIOC_PRIVATE + 5, int)
#define VIDIOC_DEFAULT_CMD_ISP_TUNING	 _IOWR('V', BASE_VIDIOC_PRIVATE + 6, struct isp_image_tuning_default_ctrl)
#define VIDIOC_DEFAULT_CMD_LISTEN_BUF	 _IOR('V', BASE_VIDIOC_PRIVATE - 1, int)
#define VIDIOC_SET_DEFAULT_BIN_PATH	 _IOWR('V',BASE_VIDIOC_PRIVATE + 7, int)
#define VIDIOC_GET_DEFAULT_BIN_PATH	 _IOWR('V',BASE_VIDIOC_PRIVATE + 8, int)

#define VIDIOC_CREATE_SUBDEV_LINKS	 		_IOW('V', BASE_VIDIOC_PRIVATE + 16, int)
#define VIDIOC_DESTROY_SUBDEV_LINKS	 		_IOW('V', BASE_VIDIOC_PRIVATE + 17, int)
#define VIDIOC_LINKS_STREAMON	 			_IOW('V', BASE_VIDIOC_PRIVATE + 18, int)
#define VIDIOC_LINKS_STREAMOFF	 			_IOW('V', BASE_VIDIOC_PRIVATE + 19, int)
#define VIDIOC_SET_BUF_INFO             _IOW('V', BASE_VIDIOC_PRIVATE + 20, struct isp_buf_info)
#define VIDIOC_GET_BUF_INFO             _IOW('V', BASE_VIDIOC_PRIVATE + 21, struct isp_buf_info)
#define VIDIOC_SET_WDR_BUF_INFO	 		    	  _IOW('V', BASE_VIDIOC_PRIVATE + 22, struct isp_buf_info)
#define VIDIOC_GET_WDR_BUF_INFO	 		    	  _IOW('V', BASE_VIDIOC_PRIVATE + 23, struct isp_buf_info)
#define VIDIOC_ISP_WDR_ENABLE                             _IOW('V', BASE_VIDIOC_PRIVATE + 24, int)
#define VIDIOC_ISP_WDR_DISABLE                             _IOW('V', BASE_VIDIOC_PRIVATE + 25, int)

#define TISP_VIDIOC_SET_AE_ALGO_FUNC                            _IOW('V',BASE_VIDIOC_PRIVATE + 26, int)
#define TISP_VIDIOC_GET_AE_ALGO_HANDLE                          _IOW('V',BASE_VIDIOC_PRIVATE + 27, int)
#define TISP_VIDIOC_SET_AE_ALGO_HANDLE                          _IOW('V',BASE_VIDIOC_PRIVATE + 28, int)
#define TISP_VIDIOC_SET_AE_ALGO_OPEN                            _IOW('V',BASE_VIDIOC_PRIVATE + 29, int)
#define TISP_VIDIOC_SET_AE_ALGO_CLOSE                           _IOW('V',BASE_VIDIOC_PRIVATE + 30, int)
#define TISP_VIDIOC_SET_AE_ALGO_CTRL                            _IOW('V',BASE_VIDIOC_PRIVATE + 31, int)
#define TISP_VIDIOC_SET_AWB_ALGO_FUNC				_IOWR('V',BASE_VIDIOC_PRIVATE + 32, int)
#define TISP_VIDIOC_GET_AWB_ALGO_HANDLE				_IOWR('V',BASE_VIDIOC_PRIVATE + 33, int)
#define TISP_VIDIOC_SET_AWB_ALGO_HANDLE				_IOWR('V',BASE_VIDIOC_PRIVATE + 34, int)
#define TISP_VIDIOC_SET_AWB_ALGO_OPEN				_IOWR('V',BASE_VIDIOC_PRIVATE + 35, int)
#define TISP_VIDIOC_SET_AWB_ALGO_CLOSE				_IOWR('V',BASE_VIDIOC_PRIVATE + 36, int)
#define TISP_VIDIOC_SET_AWB_ALGO_CTRL				_IOWR('V',BASE_VIDIOC_PRIVATE + 37, int)
#define TISP_VIDIOC_SET_FRAME_DROP                              _IOWR('V',BASE_VIDIOC_PRIVATE + 38, int)
#define TISP_VIDIOC_GET_FRAME_DROP                              _IOWR('V',BASE_VIDIOC_PRIVATE + 39, int)

#define VIDIOC_SET_GPIO_INIT                              _IOWR('V',BASE_VIDIOC_PRIVATE + 40, int)
#define VIDIOC_SET_GPIO_STA                              _IOWR('V',BASE_VIDIOC_PRIVATE + 41, int)

#define TISP_VIDIOC_SET_FRAME_DROP1				_IOWR('V',BASE_VIDIOC_PRIVATE + 42, int)
#define TISP_VIDIOC_GET_FRAME_DROP1				_IOWR('V',BASE_VIDIOC_PRIVATE + 43, int)
#define TISP_VIDIOC_STREAM_CHECK   				_IOWR('V',BASE_VIDIOC_PRIVATE + 44, int)
#define TISP_VIDIOC_SET_STREAM_OUT 				_IOWR('V',BASE_VIDIOC_PRIVATE + 45, int)

//ivdc ioctl
#define BASE_IVDC_PRIVATE                       60
#define TISP_VIDIOC_SET_JPEG                    _IO('T', BASE_IVDC_PRIVATE + 1)
#define TISP_VIDIOC_SET_H264_H265               _IO('T', BASE_IVDC_PRIVATE + 2)
#define TISP_VIDIOC_GET_DIRECT                  _IOWR('T', BASE_IVDC_PRIVATE + 3, int)
#define TISP_VIDIOC_SET_DIRECT                  _IOWR('T', BASE_IVDC_PRIVATE + 4, int)
#define TISP_VIDIOC_SET_PADDR                   _IOWR('T', BASE_IVDC_PRIVATE + 5, unsigned int)
#define TISP_VIDIOC_MEM_LINE                    _IOWR('T', BASE_IVDC_PRIVATE + 6, int)

static inline int imppixfmt_to_v4l2pixfmt(IMPPixelFormat imp_pixfmt)
{
	int v4l2_pixfmt = 0;
	switch (imp_pixfmt) {
	case PIX_FMT_NV12:
		v4l2_pixfmt = V4L2_PIX_FMT_NV12;
		break;
	case PIX_FMT_YUYV422:
		v4l2_pixfmt = V4L2_PIX_FMT_YUYV;
		break;
	case PIX_FMT_UYVY422:
		v4l2_pixfmt = V4L2_PIX_FMT_UYVY;
		break;
	case PIX_FMT_RGB565BE:
		v4l2_pixfmt = V4L2_PIX_FMT_RGB565;
	case PIX_FMT_BGR0:
		v4l2_pixfmt = V4L2_PIX_FMT_BGR32;
		break;
	case PIX_FMT_BGR24:
		v4l2_pixfmt = V4L2_PIX_FMT_BGR24;
		break;
	default:
		v4l2_pixfmt = -1;
		break;
	}
	return v4l2_pixfmt;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

/**
 * @}
 */

#endif /* __EMU_V4L2_H__ */
