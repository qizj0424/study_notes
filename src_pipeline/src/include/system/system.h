/*
 * IMP system implement function header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 * Author: Zoro <ykli@ingenic.cn>
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdbool.h>
#include <mxu2.h>

#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <isp/ncu_common.h>
#include <imp/imp_isp.h>

#define FAKEDEV_MAX 8
#define FRAME_TYPE_IVDC (3)

enum {
	DEV_ID_EMU_FS = DEV_ID_RESERVED_START,		/**< 模拟视频源 */
	DEV_ID_EMU_ENC,								/**< 模拟编码器 */
	DEV_ID_EMU_OSD,								/**< OSD */
#define DEV_ID_EMU_FAKE(n) (DEV_ID_EMU_FAKE_START + n)
	DEV_ID_EMU_FAKE_START,
	DEV_ID_EMU_FAKE_END = DEV_ID_EMU_FAKE(FAKEDEV_MAX),
#define NR_FAKE_DEV (FAKEDEV_MAX + 1)
};

typedef enum {
	RAW,
	PROCESS,
	THREAD
} IMPTimeType;

typedef struct {
	uint32_t	i_fps_num;
	uint32_t	i_fps_den;
	uint32_t	b_use_ncuinfo;
	uint32_t	b_use_aeinfo;
	IMPNCUFrameInfo ncuinfo;
	IMPISPZone ae_zone;
	uint32_t qcount;
	uint32_t dqcount;
	uint32_t ivdc_overflow_cnt;
	uint32_t data_threshold;
	uint8_t  frame_type;
	uint8_t  sensor_id;
} IMPFrameInfoPriv;

typedef struct {
	uint32_t addr;
	uint32_t value;
	uint32_t is_write;
} group_regrw;

static inline void *frameInfo_pri(IMPFrameInfo *frame)
{
	return (void *)frame->priv;
}

typedef struct IMPFrameInfoPadPrivData {
	IMPFrameInfo		frame;
	IMPFrameInfoPriv	priv;
} IMPFrameInfoPadPrivData;

typedef enum {
	SOC_UNKNOWN = -1,
	SOC_T10_START = 0,
	SOC_T10_NORMAL = SOC_T10_START,
	SOC_T10_LITE,
	SOC_T10_LITE_V2,
	SOC_T10_END = SOC_T10_LITE_V2,
	SOC_T20_START,
	SOC_T20_NORMAL = SOC_T20_START,
	SOC_T20_LITE,
	SOC_T20_X,
	SOC_T20_END = SOC_T20_X,
	SOC_T30_START,
	SOC_T30_LITE = SOC_T30_START,
	SOC_T30_N,
	SOC_T30_X,
	SOC_T30_A,
	SOC_T30_Z,
	SOC_T30_END = SOC_T30_Z,
	SOC_T21_START,
	SOC_T21_L = SOC_T21_START,
	SOC_T21_N,
	SOC_T21_X,
	SOC_T21_Z,
	SOC_T21_END = SOC_T21_Z,
	SOC_T23_START,
	SOC_T23_N = SOC_T23_START,
	SOC_T23_ZN,
	SOC_T23_END = SOC_T23_ZN,
} IMPCPUID;

/*
 * Usage:
 * if (IS_SOC_SERIES(T10)) {
 *     ...
 * } else if (IS_SOC_SERIES(T20)) {
 *     ...
 * } else if ... {
 *     ...
 * }
 */
#define IS_SOC_SERIES(ID) ((SOC_##ID##_START <= get_cpu_id()) && (get_cpu_id() <= SOC_##ID##_END))

int system_init(void);
int system_exit(void);
int64_t system_gettime(IMPTimeType type);
int system_rebasetime(int64_t base);
int system_bind(IMPCell *srcCell, IMPCell *dstCell);
int system_unbind(IMPCell *srcCell, IMPCell *dstCell);
int system_get_bind_src(IMPCell *dstCell, IMPCell *srcCell);
int system_attach(IMPCell *fromCell, IMPCell *toCell);
int system_detach(IMPCell *fromCell, IMPCell *toCell);
int system_bind_dump(void);
char *pixfmt_to_string(IMPPixelFormat pixfmt);
//inline long int mygettid(void);
int read_reg_32(uint32_t addr, uint32_t *value);
int write_reg_32(uint32_t addr, uint32_t value);
int get_cpuinfo(uint32_t addr, uint32_t *value);
IMPCPUID get_cpu_id(void);
bool is_has_simd128(void);

/* A key to MXUv2. */
static inline void mk(void)
{
	__asm__ __volatile__(												\
		"	.set	push\n"												\
		"	.set	noat\n"												\
		"	.word	0x7000000c | (0x0c << 11) | (0 << 6)\n"				\
		"	.word	0x7000000c | (0x03 << 11) | (1 << 6)\n"				\
		"	.word	0x7000000c | (0x00 << 11) | (2 << 6)\n"				\
		"	.word	0x7000000c | (0x09 << 11) | (3 << 6)\n"				\
		"	.set	pop\n"
		);
}


#endif /* __SYSTEM_H__ */
