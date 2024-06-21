/*
 *  IMP Memory alloc and management module header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 * Author: Aeolus <weiqing.yan@ingenic.com>
 */

#ifndef __IMP_ALLOC_H__
#define __IMP_ALLOC_H__

#include <stdint.h>

#include <constraints.h>
#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

typedef enum {
	MEM_ATTR_NONE,
	MEM_ATTR_DIRTY,
	MEM_ATTR_CACHED,
} IMPMemAttr;

typedef struct {
	char famliy[MAX_NAME_LEN];
	char mode[MAX_NAME_LEN];
	char owner[MAX_NAME_LEN];
	uint32_t vaddr;
	uint32_t paddr;
	int length;
	int ref_cnt;
	IMPMemAttr mem_attr;
} IMPAllocInfo;

typedef struct {
	char dev_name[MAX_NAME_LEN];
	IMPAllocInfo info;
} IMPAlloc;

int IMP_Alloc(IMPAlloc *alloc, int size, char *owner);
int IMP_Sp_Alloc(IMPAlloc *alloc, void *ptr);
void IMP_Free(IMPAlloc *alloc, void *ptr);
IMPMemAttr IMP_Alloc_Get_Attr(IMPAlloc *alloc);
void *IMP_Virt_to_Phys(void *vaddr);
void *IMP_Phys_to_Virt(void *paddr);
int IMP_Alloc_Set_Attr(IMPAlloc *alloc);
void IMP_Alloc_Dump();
int IMP_FlushCache(void *vaddr, size_t size, uint32_t dir);

int IMP_PoolAlloc(int poolId, IMPAlloc *alloc, int size, char *owner);
void IMP_PoolFree(int poolId, IMPAlloc *alloc, void *ptr);

IMPMemAttr IMP_PoolAlloc_Get_Attr(int poolId, IMPAlloc *alloc);
void *IMP_PoolVirt_to_Phys(int poolId, void *vaddr);
void *IMP_PoolPhys_to_Virt(int poolId, void *paddr);
int IMP_PoolAlloc_Set_Attr(int poolId, IMPAlloc *alloc);
void IMP_PoolAlloc_Dump(int poolId);
int IMP_PoolFlushCache(int poolId, void *vaddr, size_t size, uint32_t dir);

typedef struct
{
	int poolId;
	uint32_t virBase;
	uint32_t phyBase;
	int size;
	IMPAlloc *pAlloc;
	void *CodecAllocator;
	void (*CodecAllocatorDeinitCB)(void *CodecAllocator);
} mempool_t;

int IMP_MemPool_InitPool(int poolId, size_t size, char *name);
int IMP_MemPool_Release(int poolId);
mempool_t* IMP_MemPool_GetById(int poolId);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IMP_ALLOC_H__ */
