/*
 * Ingenic IMP System Control module implement.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 * Author: Zoro <ykli@ingenic.cn>
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>

#include <constraints.h>
#include <imp/imp_utils.h>
#include <imp/imp_log.h>
#include <imp/imp_system.h>

#include <system/module.h>
#include <system/group.h>
#include <system/system.h>

#include <dsystem/funcs.h>
#ifdef DMALLOC
#include <imp/dmalloc.h>
#endif
#define TAG "System"

#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW		4
#endif

#define SYS_MXU_CPUINFO			"/proc/cpuinfo"
#define SYS_MXU_LINE_MAX_LEN	512
#define SYS_MXU_MAGIC			"ASEs implemented"
#define SYS_MXU_VAL				"mxu_v2"
#define SYS_DEVMEM_BASE_ADDR    0x10000000
#define SYS_DEVMEM_MAP_SIZE     0x4000000

extern Module *g_modules[][NR_MAX_GROUPS_IN_DEV];

int64_t timestamp_base = 0;
uint32_t g_devmem_base_vaddr = 0;
uint32_t g_devmem_base_paddr = 0;
int g_devmem_fd = -1;

struct system_function
{
	char name[MAX_NAME_LEN];
	int (*init_func)(void);
	int (*exit_func)(void);
	uint64_t support_soc;
};

#define SYSTEM_FUNC(NAME, INIT_FUNC, EXIT_FUNC, SOC)						\
	{ .name = NAME, .init_func = INIT_FUNC, .exit_func = EXIT_FUNC, .support_soc = SOC }
#define EXTERN_SYS_FUNC(F1, F2)		extern int F1(void); extern int F2(void)
#define SOC_TYPE(T) (1 << T)

EXTERN_SYS_FUNC(EmuFrameSourceInit, EmuFrameSourceExit);
EXTERN_SYS_FUNC(EmuEncoderInit, EmuEncoderExit);
EXTERN_SYS_FUNC(EmuFakedevInit, EmuFakedevExit);
EXTERN_SYS_FUNC(FrameSourceInit, FrameSourceExit);
EXTERN_SYS_FUNC(IVSInit, IVSExit);
EXTERN_SYS_FUNC(OSDInit, OSDExit);
EXTERN_SYS_FUNC(EncoderInit, EncoderExit);
EXTERN_SYS_FUNC(DecoderInit, DecoderExit);
EXTERN_SYS_FUNC(FBInit, FBExit);
EXTERN_SYS_FUNC(DsystemInit, DsystemExit);

static int regrw_group(group_regrw *regrw_group, uint32_t count);

struct system_function sys_funcs[] = {
	SYSTEM_FUNC("DSystem", DsystemInit, DsystemExit,
				SOC_TYPE(SOC_T10_NORMAL) | SOC_TYPE(SOC_T10_LITE) | SOC_TYPE(SOC_T10_LITE_V2)),
	SYSTEM_FUNC("FrameSource", FrameSourceInit, FrameSourceExit,
				SOC_TYPE(SOC_T10_NORMAL) | SOC_TYPE(SOC_T10_LITE) | SOC_TYPE(SOC_T10_LITE_V2)),
	SYSTEM_FUNC("IVS", IVSInit, IVSExit,
				SOC_TYPE(SOC_T10_NORMAL) | SOC_TYPE(SOC_T10_LITE) | SOC_TYPE(SOC_T10_LITE_V2)),
	SYSTEM_FUNC("OSD", OSDInit, OSDExit,
				SOC_TYPE(SOC_T10_NORMAL) | SOC_TYPE(SOC_T10_LITE) | SOC_TYPE(SOC_T10_LITE_V2)),
	SYSTEM_FUNC("Encoder", EncoderInit, EncoderExit,
				SOC_TYPE(SOC_T10_NORMAL) | SOC_TYPE(SOC_T10_LITE) | SOC_TYPE(SOC_T10_LITE_V2)),
	SYSTEM_FUNC("Decoder", DecoderInit, DecoderExit,
				SOC_TYPE(SOC_T21_N)),
	SYSTEM_FUNC("FB", FBInit, FBExit,
				SOC_TYPE(SOC_T21_N)),
};

static inline int64_t timespec_to_us(struct timespec *time)
{
	return (int64_t)time->tv_sec * 1000000 + (int64_t)time->tv_nsec / 1000;
}


int system_init(void)
{
	int i, ret = 0;

	printf("%s()\n", __func__);

	/* Init base timestamp */
	struct timespec base_time;
	ret = clock_gettime(CLOCK_MONOTONIC_RAW, &base_time);
	if (ret < 0) {
		printf("Time init error\n");
		return -1;
	}

	timestamp_base = timespec_to_us(&base_time);

	IMPCPUID this_soc = get_cpu_id();

	for (i = 0; i < ARRAY_SIZE(sys_funcs); i++) {
		if (1/*sys_funcs[i].support_soc & SOC_TYPE(this_soc)*/) { /* Check soc support */
			printf("Calling %s\n", sys_funcs[i].name);

			ret = sys_funcs[i].init_func();
			if (ret < 0) {
				printf("%s failed\n", sys_funcs[i].name);
				break;
			}
		}
	}

	if (ret < 0) {
		int j;
		for (j = i - 1; j >=0; j--) {
			if (1/*sys_funcs[i].support_soc & SOC_TYPE(this_soc)*/)
				sys_funcs[i].exit_func();
		}
	}

	return ret;
}

int system_exit(void)
{
	int i;

	printf("%s\n", __func__);

	IMPCPUID this_soc = get_cpu_id();
	for (i = 0; i < ARRAY_SIZE(sys_funcs); i++) {
		if (1/*sys_funcs[i].support_soc & SOC_TYPE(this_soc)*/) { /* Check soc support */
			printf("Calling %s\n", sys_funcs[i].name);
			sys_funcs[i].exit_func();
		}
	}

#if 0
	sys_devmem_unmap();
#endif

	return 0;
}

int64_t system_gettime(IMPTimeType type)
{
	clockid_t clk_id;
	switch (type) {
	case RAW:
		clk_id = CLOCK_MONOTONIC_RAW;
		break;
	case PROCESS:
		clk_id = CLOCK_PROCESS_CPUTIME_ID;
		break;
	case THREAD:
		clk_id = CLOCK_THREAD_CPUTIME_ID;
		break;
	default:
		return -1;
	}

	struct timespec time;
	int ret = clock_gettime(clk_id, &time);
	if (ret < 0)
		return 0;

	int64_t now = timespec_to_us(&time);
	now -= timestamp_base;

	return now;
}

int system_get_localtime(char *buf, int size)
{
	struct tm tm;
	time_t t;

	if (buf == NULL || size < 21) {
		printf("%s buf is null or size less than 21\n", __func__);
		return -1;
	}

	t = time(NULL);
	if (localtime_r((const time_t *)&t, &tm) == NULL) {
		printf("%s localtime_r failed\n", __func__);
		return -1;
	}
	sprintf(buf, "%04d%02d%02d%02d%02d%02d", tm.tm_year + 1900,
			tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	return 0;
}

int system_rebasetime(int64_t base)
{
	struct timespec raw_time;
	int ret = clock_gettime(CLOCK_MONOTONIC_RAW, &raw_time);
	if (ret < 0)
		return -1;

	timestamp_base = timespec_to_us(&raw_time) - base;

	return 0;
}

int system_bind(IMPCell *srcCell, IMPCell *dstCell)
{
	Module *module_src = get_module(srcCell->deviceID, srcCell->groupID);
	Module *module_dst = get_module(dstCell->deviceID, dstCell->groupID);

	if (module_src == NULL) {
		printf("%s() error: invalid src channel(%d.%d.%d)\n", __func__,
					srcCell->deviceID, srcCell->groupID, srcCell->outputID);
		return -1;
	}

	if (module_dst == NULL) {
		printf("%s() error: invalid dst channel(%d.%d.%d)\n", __func__,
					dstCell->deviceID, dstCell->groupID, dstCell->outputID);
		return -1;
	}

	printf("%s(): bind DST-%s(%d.%d.%d) to SRC-%s(%d.%d.%d)\n", __func__,
				module_dst->name, dstCell->deviceID, dstCell->groupID, dstCell->outputID,
				module_src->name, srcCell->deviceID, srcCell->groupID, srcCell->outputID);

	/* xhshen : srcCell->outputID >= group->nr_outputs ?? */
	Group *group = (Group *)module_pri(module_src);
	if (srcCell->outputID >= group->nr_outputs) {
		printf("%s() error: invalid SRC:%s()\n", __func__,
					module_src->name, srcCell->deviceID, srcCell->groupID, srcCell->outputID);
		return -1;
	}

	void **data = (void **)(&group->for_output_data[srcCell->outputID]);

	BindObserverToSubject(module_src, module_dst, data);

	return 0;
}

int system_unbind(IMPCell *srcCell, IMPCell *dstCell)
{
	Module *module_src = get_module(srcCell->deviceID, srcCell->groupID);
	Module *module_dst = get_module(dstCell->deviceID, dstCell->groupID);

	if (module_src == NULL) {
		printf("%s() error: invalid src channel(%d.%d.%d)\n", __func__,
					srcCell->deviceID, srcCell->groupID, srcCell->outputID);
		return -1;
	}

	if (module_dst == NULL) {
		printf("%s() error: invalid dst channel(%d.%d.%d)\n", __func__,
					dstCell->deviceID, dstCell->groupID, dstCell->outputID);
		return -1;
	}

	printf("%s(): unbind DST-%s(%d.%d.%d) from SRC-%s(%d.%d.%d)\n", __func__,
				module_dst->name, dstCell->deviceID, dstCell->groupID, dstCell->outputID,
				module_src->name, srcCell->deviceID, srcCell->groupID, srcCell->outputID);

	UnBindObserverFromSubject(module_src, module_dst);

	return 0;
}

int system_get_bind_src(IMPCell *dstCell, IMPCell *srcCell)
{
	int ret;
	Module *module_dst = get_module(dstCell->deviceID, dstCell->groupID);
	if (module_dst == NULL) {
		printf("%s() error: invalid dst channel\n", __func__);
		return -1;
	}

	Module *module_src = module_subject(module_dst);
	if (module_src == NULL) {
		printf("%s() error: dst channel(%s) has not been binded\n",
					__func__, module_dst->name);
		return -1;
	}

	srcCell->outputID = module_ob_id(module_src, module_dst);

	ret = get_module_location(module_src, &srcCell->deviceID, &srcCell->groupID);

	return ret;
}

int system_attach(IMPCell *fromCell, IMPCell *toCell)
{
	int ret;
	IMPCell srcCell;

	ret = system_get_bind_src(toCell, &srcCell);
	if (ret < 0)
		goto err1;

	ret = system_unbind(&srcCell, toCell);
	if (ret < 0)
		goto err1;

	ret = system_bind(&srcCell, fromCell);
	if (ret < 0)
		goto err2;

	ret = system_bind(fromCell, toCell);
	if (ret < 0)
		goto err3;

	return ret;
err3:
	system_unbind(&srcCell, fromCell);
err2:
	system_bind(&srcCell, toCell);
err1:
	printf("%s() error: attach %s error\n", __func__,
				get_module(toCell->deviceID, toCell->groupID)->name);
	return ret;
}

int system_detach(IMPCell *fromCell, IMPCell *toCell)
{
	int ret;
	IMPCell srcCell;

	ret = system_get_bind_src(fromCell, &srcCell);
	if (ret < 0)
		goto err1;

	ret = system_unbind(&srcCell, fromCell);
	if (ret < 0)
		goto err1;

	ret = system_unbind(fromCell, toCell);
	if (ret < 0)
		goto err2;

	ret = system_bind(&srcCell, toCell);
	if (ret < 0)
		goto err3;

	return ret;
err3:
	system_bind(fromCell, toCell);
err2:
	system_bind(&srcCell, fromCell);
err1:
	printf("%s() error: detach %s error\n", __func__,
				get_module(toCell->deviceID, toCell->groupID)->name);
	return ret;
}

static void dump_ob_modules(Module *sub, int level)
{
	int i;
	char tabs[64];

	memset(tabs, 0, ARRAY_SIZE(tabs));

	for (i = 0; i < level * 4; i++) {
		tabs[i] = ' ';i++;
		tabs[i] = ' ';i++;
		tabs[i] = ' ';i++;
		tabs[i] = ' ';
	}
	tabs[i] = '\0';

	for (i = 0; i < nr_module_observer(sub); i++) {
		Module *ob = module_observer_by_num(sub, i);
		printf("%s%s-%d->%s\n", tabs, sub->name, i, ob->name);
		dump_ob_modules(ob, level + 1);
	}
}

int system_bind_dump(void)
{
	int i, j;
	Module *source = NULL;

	/* Find who is the source*/
	for (i = 0; i < NR_MAX_DEVICES; i++) {
		for (j = 0; j < NR_MAX_GROUPS_IN_DEV; j++) {
			Module *module = get_module(i, j);
			if (module) {
				printf("enumerate: %s(%d, %d)\n", module->name, i, j);
				if ((nr_module_observer(module) > 0)
					&& (module_subject(module) == NULL)) {
					source = module;
					goto out;
				}
			}
		}
	}

	if (source == NULL) {
		printf("%s() error: No source found\n", __func__);
		return -1;
	}
out:
	printf("Source:%s(%d, %d)\n", source->name, i, j);
	dump_ob_modules(source, 0);

	return 0;
}

char *pixfmt_to_string(IMPPixelFormat pixfmt)
{
	static char *fmt_str[PIX_FMT_NB] = {
		[PIX_FMT_YUV420P] = "YUV420Planar",
		[PIX_FMT_YUYV422] = "YUYV422",
		[PIX_FMT_UYVY422] = "UYVY422",
		[PIX_FMT_YUV422P] = "YUV422P",
		[PIX_FMT_YUV444P] = "YUV444P",
		[PIX_FMT_YUV410P] = "YUV410P",
		[PIX_FMT_YUV411P] = "YUV411P",
		[PIX_FMT_GRAY8] = "GRAY8",
		[PIX_FMT_MONOWHITE] = "MONOWHITE",
		[PIX_FMT_MONOBLACK] = "MONOBLACK",

		[PIX_FMT_NV12] = "NV12",
		[PIX_FMT_NV21] = "NV21",

		[PIX_FMT_RGB24] = "RGB24",
		[PIX_FMT_BGR24] = "BGR24",

		[PIX_FMT_ARGB] = "ARGB",
		[PIX_FMT_RGBA] = "RGBA",
		[PIX_FMT_ABGR] = "ABGR",
		[PIX_FMT_BGRA] = "BGRA",

		[PIX_FMT_RGB565BE] = "RGB565BE",
		[PIX_FMT_RGB565LE] = "RGB565LE",
		[PIX_FMT_RGB555BE] = "RGB555BE",
		[PIX_FMT_RGB555LE] = "RGB555LE",

		[PIX_FMT_BGR565BE] = "BGR565BE",
		[PIX_FMT_BGR565LE] = "BGR565LE",
		[PIX_FMT_BGR555BE] = "BGR555BE",
		[PIX_FMT_BGR555LE] = "BGR555LE",

		[PIX_FMT_0RGB] = "0RGB",
		[PIX_FMT_RGB0] = "RGB0",
		[PIX_FMT_0BGR] = "0BGR",
		[PIX_FMT_BGR0] = "BGR0",

		[PIX_FMT_BAYER_BGGR8] = "BGGR8",
		[PIX_FMT_BAYER_RGGB8] = "RGGB8",
		[PIX_FMT_BAYER_GBRG8] = "GBRG8",
		[PIX_FMT_BAYER_GRBG8] = "GRBG8"
	};

	if (pixfmt < PIX_FMT_NB)
		return fmt_str[pixfmt];

	return NULL;
}

inline long int mygettid(void)
{
	return syscall(SYS_gettid);
}

static int regrw_group(group_regrw *regrw_group, uint32_t count)
{
	void *map_base, *virt_addr;
	off_t target;
	unsigned page_size, mapped_size, offset_in_page;
	int fd, width = 32, ret = 0;
    unsigned reg_count = count, i = 0, max, min;

    max=min=regrw_group[0].addr;
    for(i=0;i<reg_count;i++) {
        max=regrw_group[i].addr>max?regrw_group[i].addr:max;
        min=regrw_group[i].addr<min?regrw_group[i].addr:min;
    }

    target = min;
	fd = open("/dev/mem", (O_RDWR | O_SYNC));
	if (fd < 0) {
        printf("open /dev/mem failed\n");
		return -1;
	}

	mapped_size = page_size = getpagesize();
    mapped_size += ((max-min)/page_size + ((max-min)%page_size?1:0))*page_size;
	offset_in_page = (unsigned)target & (page_size - 1);
	if (offset_in_page + width > page_size) {
		/* This access spans pages.
		 * Must map two pages to make it possible: */
		mapped_size *= 2;
	}
	map_base = mmap(NULL,
					mapped_size,
					(PROT_READ | PROT_WRITE),
					MAP_SHARED,
					fd,
					target & ~(off_t)(page_size - 1));
	if (map_base == MAP_FAILED) {
		printf("mmap failed\n");
		ret = -1;
		goto close_fd;
	}

    for(i=0;i<count;i++) {
	    virt_addr = (char*)map_base + offset_in_page + (regrw_group[i].addr-min);
	    if (!regrw_group[i].is_write) {
		    regrw_group[i].value = *(volatile uint32_t*)virt_addr;
        } else {
		    *(volatile uint32_t*)virt_addr = regrw_group[i].value;
        }
    }


	if (munmap(map_base, mapped_size) == -1)
		printf("munmap failed\n");

close_fd:
	close(fd);

	return ret;
}
#if 0
static int regrw(uint32_t addr, uint32_t *value, int is_write)
{
	int ret = -1;
	uint32_t map_base;
	uint32_t virt_addr = 0;

	if (-1 == g_devmem_fd) {
		ret = sys_devmem_map();
		if (ret < 0) {
			printf("error:(%s,%d),sys_devmem_map failed.\n",__func__,__LINE__);
			return -1;
		}
	}
	map_base = g_devmem_base_vaddr;
	virt_addr = map_base + (addr - SYS_DEVMEM_BASE_ADDR);

	 if(!is_write)
		*value = *(volatile uint32_t*)virt_addr;
	 else
		*(volatile uint32_t*)virt_addr = *value;

	 return 0;
}
#else
static int regrw(uint32_t addr, uint32_t *value, int is_write)
{
	void *map_base, *virt_addr;
	off_t target;
	unsigned page_size, mapped_size, offset_in_page;
	int fd, width = 32, ret = 0;

	target = addr;
	fd = open("/dev/mem", is_write ? (O_RDWR | O_SYNC) : (O_RDONLY | O_SYNC));
	if (fd < 0) {
		printf("open /dev/mem failed\n");
		return -1;
	}

	mapped_size = page_size = getpagesize();
	offset_in_page = (unsigned)target & (page_size - 1);
	if (offset_in_page + width > page_size) {
		/* This access spans pages.
		 * Must map two pages to make it possible: */
		mapped_size *= 2;
	}
	map_base = mmap(NULL,
					mapped_size,
					is_write ? (PROT_READ | PROT_WRITE) : PROT_READ,
					MAP_SHARED,
					fd,
					target & ~(off_t)(page_size - 1));
	if (map_base == MAP_FAILED) {
		printf("mmap failed\n");
		ret = -1;
		goto close_fd;
	}

	virt_addr = (char*)map_base + offset_in_page;

	if (!is_write)
		*value = *(volatile uint32_t*)virt_addr;
	else
		*(volatile uint32_t*)virt_addr = *value;

	if (munmap(map_base, mapped_size) == -1)
		printf("munmap failed\n");

close_fd:
	close(fd);

	return ret;
}
#endif

int read_reg_32(uint32_t addr, uint32_t *value)
{
	return regrw(addr, value, 0);
}

int write_reg_32(uint32_t addr, uint32_t value)
{
	return regrw(addr, &value, 1);
}

int get_cpuinfo(uint32_t addr, uint32_t *value)
{
	return regrw(addr, value, 0);
}

static int is_video_has_simd128_proc(void)
{
	FILE *fb = NULL;
	char linebuf[SYS_MXU_LINE_MAX_LEN], *strpoint = NULL;
	int  readsize = 0, mxu_magicsize = strlen(SYS_MXU_MAGIC), mxu_valuesize = strlen(SYS_MXU_VAL);

	fb = fopen(SYS_MXU_CPUINFO, "r");
	if (fb == NULL) {
		printf("[ignored]fopen %s returns NULL\n", SYS_MXU_CPUINFO);
		goto err_fopen;
	}

	do {
		strpoint = fgets(linebuf, SYS_MXU_LINE_MAX_LEN, fb);
		if (strpoint == NULL) {
			printf("[ignored]read %s ret is NULL", SYS_MXU_CPUINFO);
			goto err_fgets;
		}
		readsize = strlen(linebuf);
		if ((readsize > mxu_magicsize + mxu_valuesize + 1)
				&& ((strpoint = strstr(strpoint, SYS_MXU_MAGIC)) != NULL)
				&& ((strpoint = strstr(strpoint + mxu_magicsize + 1, SYS_MXU_VAL)) != NULL)) {
			break;
		}
	} while(1);

	fclose(fb);

	return 1;

err_fgets:
	fclose(fb);
err_fopen:
	return 0;
}

IMPCPUID get_cpu_id(void)
{
	return SOC_UNKNOWN;
}

static int is_video_has_simd128_readreg(void)
{
        return 0;
}

static int is_video_close_simd128(void)
{
	return !!access("/tmp/closesimd", F_OK);
}

bool is_has_simd128(void)
{
	return ((is_video_has_simd128_proc() || is_video_has_simd128_readreg()) && is_video_close_simd128());
}

static int is_support_simd128_readreg(void)
{
	IMPCPUID cpu_id = get_cpu_id();
	return (cpu_id == SOC_T10_NORMAL) || (cpu_id == SOC_T20_NORMAL) || ((cpu_id >= SOC_T30_N)&&(cpu_id <= SOC_T30_Z));
}

bool is_support_simd128(void)
{
	return ((is_video_has_simd128_proc() || is_support_simd128_readreg()) && is_video_close_simd128());
}
