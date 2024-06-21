/*
 * Ingenic IMP System Control module implement.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 * Author: Zoro <ykli@ingenic.cn>
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <constraints.h>
#include <imp/imp_log.h>
#include <imp/imp_system.h>

#include <system/module.h>
#include <system/group.h>
#include <system/system.h>
#ifdef DMALLOC
#include <imp/dmalloc.h>
#endif
#define TAG "System"

int IMP_System_Init(void)
{
	return system_init();
}

int IMP_System_Exit(void)
{
	printf("%s\n", __func__);
	return system_exit();
}

int64_t IMP_System_GetTimeStamp(void)
{
	return system_gettime(RAW);
}

int IMP_System_RebaseTimeStamp(int64_t basets)
{
	return system_rebasetime(basets);
}

uint32_t IMP_System_ReadReg32(uint32_t regAddr)
{
	uint32_t read_value;
	int ret = read_reg_32(regAddr, &read_value);

	if (ret < 0) {
		read_value = 0;
		printf("%s(): failed\n");
	}

	return read_value;
}

void IMP_System_WriteReg32(uint32_t regAddr, uint32_t value)
{
	int ret = write_reg_32(regAddr, value);

	if (ret < 0)
		printf("%s(): failed\n");
}

int IMP_System_GetVersion(IMPVersion *pstVersion)
{
	return 0;
}

int IMP_System_Bind(IMPCell *srcCell, IMPCell *dstCell)
{
	if (srcCell == NULL) {
		printf("%s(): src channel is NULL\n", __func__);
		return -1;
	}
	if (dstCell == NULL) {
		printf("%s(): dst channel is NULL\n", __func__);
		return -1;
	}

	return system_bind(srcCell, dstCell);
}

int IMP_System_UnBind(IMPCell *srcCell, IMPCell *dstCell)
{
	if (srcCell == NULL) {
		printf("%s(): src channel is NULL\n", __func__);
		return -1;
	}
	if (dstCell == NULL) {
		printf("%s(): dst channel is NULL\n", __func__);
		return -1;
	}

	return system_unbind(srcCell, dstCell);
}

int IMP_System_GetBindbyDest(IMPCell *dstCell, IMPCell *srcCell)
{
	if (dstCell == NULL) {
		printf("%s(): dst channel is NULL\n", __func__);
		return -1;
	}

	if (srcCell == NULL) {
		printf("%s(): src channel is NULL\n", __func__);
		return -1;
	}

	return system_get_bind_src(dstCell, srcCell);
}

const char* IMP_System_GetCPUInfo(void)
{
	return "Unknown";
}

char *IMPPixfmtToString(IMPPixelFormat pixfmt)
{
	return pixfmt_to_string(pixfmt);
}

int IMP_System_MemPoolRequest(int poolId, size_t size, char *name)
{
    return 0;//IMP_MemPool_InitPool(poolId, size, name);
}
