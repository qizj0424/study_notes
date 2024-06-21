/*
 * IMP NCU header file.
 *
 * Copyright (C) 2017 Ingenic Semiconductor Co.,Ltd
 * Author: bob <xiaobo.yu@ingenic.cn>
 */

#ifndef __NCU_COMMON_H__
#define __NCU_COMMON_H__
#include <stdint.h>

typedef struct {
	uint32_t width;
	uint32_t height;
	uint32_t frm_num;
	uint32_t sta_y_block_size;
	uint32_t sta_y_stride;
	uint32_t sta_y_buf_size;
	uint8_t *sta_y_buf;

} IMPNCUFrameInfo;

#endif
