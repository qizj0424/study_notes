/*
 * Video Buffer Manager header file.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 * Author: Zoro <ykli@ingenic.cn>
 */

#ifndef __VBM_H__
#define __VBM_H__

#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <linux/videodev2.h>
#include <linux/v4l2-mediabus.h>

#include "imp_alloc.h"
typedef struct VBMPoolConfig VBMPoolConfig;
typedef struct VBMInterface VBMInterface;
typedef struct VBMPool VBMPool;
typedef struct VBM VBM;
typedef struct FrameVolume FrameVolume;
#define IVDC_DIRECT_PHYADDR (0x8 << 28 | 0 << 25)

struct VBMPoolConfig
{
	struct v4l2_format fmt;											/**< 图片格式 */
	int nr_buffers;												/**< 每个Pool中Video Buffer的个数 */
};

struct VBMInterface {
	int (*getFrame)(IMPFrameInfo *frame, void *pri);	/**< 获得一帧的接口函数 */
	int (*releaseFrame)(IMPFrameInfo *frame, void *pri);		/**< 释放一帧的接口函数 */
};

struct VBMPool {
	int id;														/**< Pool序号 */
	void *pri;													/**< Pool私有指针 */
	VBMPoolConfig config;										/**< Pool配置 */
	IMPAlloc mem;											/**< Pool Memory 结构体 */
	uint32_t vaddr_base;										/**< Pool Memory 虚拟基地址 */
	uint32_t paddr_base;										/**< Pool Memory 物理基地址 */

	VBMInterface interface;										/**< VBM 接口 */
	int memPoolId;

	IMPFrameInfo frames[0];										/**< Pool Frame数组 */
};

struct VBM {
	VBMPool *pools[NR_MAX_FS_GROUPS];							/**< VBM Pool */
	unsigned int is_close_ncu;
};

/**
 * Frame容器，用来map frame指针，虚拟地址，物理地址，引用计数等
 */
struct FrameVolume {
	IMPFrameInfo *frame;										/**< IMPFrameInfo指针 */
	uint32_t vaddr;												/**< frame的虚拟地址 */
	uint32_t paddr;												/**< frame的物理地址 */
	int ref;													/**< frame的引用计数 */
	pthread_mutex_t mutex;	/**< 用于帧状态的同步 */
};

/**
 * 创建VBM Pool
 *
 * @fn int VBMCreatePool(int poolID, VBMPoolConfig *poolCfg, void *pri)
 *
 * @param[in] poolID Pool ID
 * @param[in] poolCfg Pool 配置
 * @param[in] pool私有数据指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 调用此函数会申请pool内存，并将pool内的frame设置好地址及格式等信息
 * @remark pri作为私有指针，在调用VBM接口时会传入VBMInterface的函数
 *
 * @attention 无
 */
int VBMCreatePool(int poolID, VBMPoolConfig *poolCfg, VBMInterface *interface, void *pri);

/**
 * 销毁VBM Pool
 *
 * @fn int VBMDestroyPool(int poolID)
 *
 * @param[in] poolID Pool ID
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 销毁pool后会释放pool包括内存在内的所有资源
 *
 * @attention 无
 */
int VBMDestroyPool(int poolID);

/**
 * 等待VBM Pool中需释放的frame索引值变为0
 *
 * @fn int VBMFlushFrame(int poolID)
 *
 * @param[in] poolID Pool ID
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 等待VBM Pool中需释放的frame索引值变为0，以10s为最长等待时间
 *
 * @attention 无
 */
int VBMFlushFrame(int poolID);

/**
 * 填充Pool的Frame
 *
 * @fn int VBMFillPool(int poolID)
 *
 * @param[in] poolID Pool ID
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 填充pool的所有frame，一般在将要获取frame之前使用
 *
 * @attention 无
 */
int VBMFillPool(int poolID);

/**
 * 清除Pool的Frame
 *
 * @fn int VBMFlushPool(int poolID)
 *
 * @param[in] poolID Pool ID
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 清空pool的所有frame，一般在channel disable之后使用，确保channel的frame不存在残留帧
 *
 * @attention 无
 */
int VBMFlushPool(int poolID);

/**
 * 获得一个Pool的Frame
 *
 * @fn int VBMGetFrame(int poolID, IMPFrameInfo **frame)
 *
 * @param[in] poolID Pool ID
 * @param[out] frame frame指针的地址
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 无
 *
 * @attention 无
 */
int VBMGetFrame(int poolID, IMPFrameInfo **frame);

/**
 * 释放一个Pool的Frame
 *
 * @fn int VBMReleaseFrame(IMPFrameInfo *frame)
 *
 * @param[in] frame frame指针
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 无
 *
 * @attention 无
 */
int VBMReleaseFrame(IMPFrameInfo *frame);

int VBMLockFrame(IMPFrameInfo *frame);
int VBMUnLockFrame(IMPFrameInfo *frame);

/**
 * Lock Frame 通过虚拟地址
 *
 * @fn int VBMLockFrameByVaddr(uint32_t vaddr)
 *
 * @param[in] vaddr 虚拟地址
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark Lock Frame之后，该Frame不会被重新填充，直到该frame的所有lock都被解除
 *
 * @attention 若无法找到匹配此虚拟地址的frame则返回错误
 */
int VBMLockFrameByVaddr(uint32_t vaddr);

/**
 * Unlock Frame 通过虚拟地址
 *
 * @fn int VBMUnlockFrameByVaddr(uint32_t vaddr)
 *
 * @param[in] vaddr 虚拟地址
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 无
 *
 * @attention 若无法找到匹配此虚拟地址的frame则返回错误
 */
int VBMUnlockFrameByVaddr(uint32_t vaddr);

/**
 * Lock Frame 通过物理地址
 *
 * @fn int VBMLockFrameByPaddr(uint32_t paddr)
 *
 * @param[in] paddr 物理地址
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark Lock Frame之后，该Frame不会被重新填充，直到该frame的所有lock都被解除
 *
 * @attention 若无法找到匹配此物理地址的frame则返回错误
 */
int VBMLockFrameByPaddr(uint32_t paddr);

/**
 * Unlock Frame 通过物理地址
 *
 * @fn int VBMUnlockFrameByPaddr(uint32_t paddr)
 *
 * @param[in] paddr 物理地址
 *
 * @retval 0 成功
 * @retval 非0 失败
 *
 * @remark 无
 *
 * @attention 若无法找到匹配此物理地址的frame则返回错误
 */
int VBMUnlockFrameByPaddr(uint32_t paddr);

IMPFrameInfo *VBMGetFrameInstance(int poolID, int frame_index);

/**
 * dump PoolID的使用情况
 *
 * @fn void VBMDumpPool(int poolID)
 *
 * @param[in] poolID frame池索引号
 *
 * @remark 无
 *
 * @attention poolID必须为已经存在的frame池
 */
void VBMDumpPool(int poolID);

#endif /* __VBM_H__ */
