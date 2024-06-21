/*
 * DEMOSAIC ISP header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 * Author: Huddy <huayao.li@ingenic.com>
 */

#ifndef __DEMOSAIC_H__
#define __DEMOSAIC_H__

typedef enum demosaic_isp_mode {
	DEMOSAIC_ISP_MODE_DISABLE,
	DEMOSAIC_ISP_MODE_ENABLE,
	DEMOSAIC_ISP_MODE_CUSTOM,
} DEMOSAIC_ISP_MODE_E;

int IMP_ISP_Tuning_SetSharpness_internal(unsigned char sharpness, DEMOSAIC_ISP_MODE_E mode);
int IMP_ISP_Tuning_SetContrast_internal(unsigned char contrast, DEMOSAIC_ISP_MODE_E mode);

#endif /* __DEMOSAIC_H__ */
