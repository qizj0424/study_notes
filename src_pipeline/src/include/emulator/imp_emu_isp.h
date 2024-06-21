/*
 * IMP ISP emulator header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 * Author: Niky <xianghui.shen@ingenic.com>
 */

#ifndef __IMP_EMU_ISP_H__
#define __IMP_EMU_ISP_H__

/**
 * @file
 * Encoder emulator func interface.
 */

#include <stdint.h>
#include <linux/videodev2.h>

#include <imp/imp_common.h>
#include <imp/imp_isp.h>

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


int EmuISPOpen(void);
int EmuISPClose(void);
int EmuISP_AddSensor(IMPSensorInfo *info);
int EmuISP_DelSensor(IMPSensorInfo *info);
int EmuISP_EnableSensor(void);
int EmuISP_DisableSensor(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IMP_EMU_ISP_H__ */
