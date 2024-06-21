/*
 * Ingenic IMP VBM, Video Buffer Manager.
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 * Author: Zoro <ykli@ingenic.cn>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <linux/videodev2.h>

#include <constraints.h>
#include <imp/imp_log.h>
#include <imp/imp_system.h>
#include <system/vbm.h>
#include <system/system.h>
#include <isp/ncu_common.h>
#include <imp/imp_isp.h>
#include <system/imp_alloc.h>
#define TAG "VBM"

#define NR_FRAME_VOLUME 30

FrameVolume g_framevolumes[NR_FRAME_VOLUME];
static VBM vbm_instance;

typedef struct {
	uint32_t width;
	uint32_t height;
	uint32_t sta_y_block_size;
	uint32_t sta_y_stride;
	uint32_t sta_y_buf_size;
}IMPISPNcuInfo;

extern int IMP_ISP_Tuning_GetNCUInfo(IMPISPNcuInfo *ncu_info);
extern IMPAlloc *IMP_ISP_Tuning_GetNCUAlloc();

#define GET_POOL_FRAME(ppool, idx)		((IMPFrameInfo *)(((char *)(((VBMPool *)(ppool))->frames)) \
			+ (idx) * (sizeof(IMPFrameInfo) + sizeof(IMPFrameInfoPriv))))
/**********************************************************************************/
/* int IMP_Free(IMPAlloc *alloc, int size, char *owner); */
/* void IMP_Alloc(IMPAlloc *alloc, void *ptr); */

/**********************************************************************************/

VBM *VBMGetInstance(void)
{
	return &vbm_instance;
}



static int add_frame_to_volume(IMPFrameInfo *frame)
{
	int i;
	FrameVolume *fvol;

	for (i = 0; i < NR_FRAME_VOLUME; i++) {
		if (g_framevolumes[i].frame == NULL)
			goto found;
	}
	return -1;
found:
	fvol = &g_framevolumes[i];
	fvol->frame = frame;
	fvol->vaddr = frame->virAddr;
	fvol->paddr = frame->phyAddr;
	fvol->ref = 0;
	pthread_mutex_init(&fvol->mutex, NULL);

	return 0;
}

static int del_frame_from_volume(IMPFrameInfo *frame)
{
	int i;
	for (i = 0; i < NR_FRAME_VOLUME; i++) {
		if (g_framevolumes[i].frame == frame)
			goto found;
	}
	return -1;
found:
	pthread_mutex_destroy(&g_framevolumes[i].mutex);
	memset(&g_framevolumes[i], 0, sizeof(FrameVolume));

	return 0;
}


static inline FrameVolume *frame_to_vol(IMPFrameInfo *frame)
{
	int i;

	for (i = 0; i < NR_FRAME_VOLUME; i++) {
		if (g_framevolumes[i].frame == frame)
			return &g_framevolumes[i];
	}

	return NULL;
}

static unsigned char print_cnt_vtv = 1;
static inline FrameVolume *vaddr_to_vol(uint32_t vaddr)
{
    int i;

    for (i = 0; i < NR_FRAME_VOLUME; i++) {
	if (g_framevolumes[i].vaddr == vaddr)
	    return &g_framevolumes[i];
    }
    if (print_cnt_vtv) {
	print_cnt_vtv--;
	if (i >= NR_FRAME_VOLUME) {
	    for (i = 0; i < NR_FRAME_VOLUME; i++) {
		printf("fval[%d].frame=%p\n", i, g_framevolumes[i].frame);
		printf("fval[%d].vaddr=0x%x\n", i, g_framevolumes[i].vaddr);
		printf("fval[%d].paddr=0x%x\n", i, g_framevolumes[i].paddr);
		printf("fval[%d].ref=%d\n", i, g_framevolumes[i].ref);
	    }
	}
    }

    return NULL;
}

static inline FrameVolume *paddr_to_vol(uint32_t paddr)
{
	int i;

	for (i = 0; i < NR_FRAME_VOLUME; i++) {
		if (g_framevolumes[i].paddr == paddr)
			return &g_framevolumes[i];
	}

	return NULL;
}

static inline int vbm_lock_vol(FrameVolume *fvol)
{
	pthread_mutex_lock(&fvol->mutex);
	fvol->ref++;
	pthread_mutex_unlock(&fvol->mutex);

	return 0;
}

void *vbm_get_frame(int poolID, int index)
{
	IMPFrameInfo *frame;
	VBM *vbm = VBMGetInstance();
	if (vbm->pools[poolID] == NULL)
		return NULL;
	frame = GET_POOL_FRAME(vbm->pools[poolID], index);
	return frame;
}

static inline int vbm_unlock_vol(FrameVolume *fvol)
{
	int ret = 0;
	int do_release = 0;

	pthread_mutex_lock(&fvol->mutex);
	if (fvol->ref == 0) {
		printf("%s(%d):fvol->ref=%d, fvol->frame->pool_idx=%d, fvol->frame->index=%d\n", __func__, __LINE__, fvol->ref, fvol->frame->pool_idx, fvol->frame->index);
		pthread_mutex_unlock(&fvol->mutex);
		return -1;
	}

	fvol->ref--;
	if (fvol->ref == 0) {
		do_release = 1;
	}

	if (do_release) {
		VBM *vbm = VBMGetInstance();
		int pool_id = fvol->frame->pool_idx;
		ret = vbm->pools[pool_id]->interface.releaseFrame(fvol->frame, vbm->pools[pool_id]->pri);
		if (ret < 0) {
			printf("%s:releaseFrame failed:frame->pool_idx=%d, frame->index=%d\n", __func__, fvol->frame->pool_idx, fvol->frame->index);
		}
	}
	pthread_mutex_unlock(&fvol->mutex);

	return 0;
}

int VBMLockFrame(IMPFrameInfo *frame)
{
	FrameVolume *fvol = frame_to_vol(frame);
	if (fvol == NULL)
		return -1;

	return vbm_lock_vol(fvol);
}

int VBMUnLockFrame(IMPFrameInfo *frame)
{
	FrameVolume *fvol = frame_to_vol(frame);
	if (fvol == NULL) {
		printf("fvol is NULL, frame->pool_id=%d, frame->index=%d\n", frame->pool_idx, frame->index);
		return -1;
	}

	return vbm_unlock_vol(fvol);
}

int VBMLockFrameByVaddr(uint32_t vaddr)
{
	FrameVolume *fvol = vaddr_to_vol(vaddr);
	if (fvol == NULL)
		return -1;

	return vbm_lock_vol(fvol);
}

int VBMUnlockFrameByVaddr(uint32_t vaddr)
{
	FrameVolume *fvol = vaddr_to_vol(vaddr);
	if (fvol == NULL) {
	    /*printf("vaddr=%x failed to get fvol\n", vaddr);*/
	    return -1;
	}

	return vbm_unlock_vol(fvol);
}

int VBMLockFrameByPaddr(uint32_t paddr)
{
	FrameVolume *fvol = paddr_to_vol(paddr);
	if (fvol == NULL)
		return -1;

	return vbm_lock_vol(fvol);
}

int VBMUnlockFrameByPaddr(uint32_t paddr)
{
	FrameVolume *fvol = paddr_to_vol(paddr);
	if (fvol == NULL)
		return -1;

	return vbm_unlock_vol(fvol);
}

static int calculate_sizeimage(int w, int h, uint32_t fourcc)
{
	int depth = 0;
	int sizeimage = 0;
	switch (fourcc) {
	case V4L2_PIX_FMT_NV12:
	case V4L2_PIX_FMT_YUV420:
		depth = 12;
		sizeimage = ((((w + 15) & ~15) * depth) >> 3) * ((h + 15) & ~15);
		break;
	case V4L2_PIX_FMT_RGB565:
	case V4L2_PIX_FMT_YUYV:
	case V4L2_PIX_FMT_UYVY:
	case V4L2_PIX_FMT_SBGGR12:
	case V4L2_PIX_FMT_SGBRG12:
	case V4L2_PIX_FMT_SGRBG12:
	case V4L2_PIX_FMT_SRGGB12:
		depth = 16;
		sizeimage = (w * h * depth) >> 3;
		break;
	case V4L2_PIX_FMT_BGR32:
		depth = 32;
		sizeimage = (w * h * depth) >> 3;
		break;
	case V4L2_PIX_FMT_BGR24:
		depth = 24;
		sizeimage = (w * h * depth) >> 3;
		break;
	default:
		return -1;
	}
	return sizeimage;
}

extern int ncu_get_frame_info(IMPNCUFrameInfo *info);
int VBMCreatePool(int poolID, VBMPoolConfig *poolCfg, VBMInterface *interface, void *pri)
{
	int ret = 0, sizeimage = 0, i = 0;
	if (poolID > NR_MAX_FS_GROUPS)
		return -1;


	VBM *vbm = VBMGetInstance();

	int alloc_size = sizeof(VBMPool) + (sizeof(IMPFrameInfo) + sizeof(IMPFrameInfoPriv)) * poolCfg->nr_buffers;

	VBMPool *pool = (VBMPool *)malloc(alloc_size);
	if (pool == NULL)
		return -1;

	if (!interface) {
		printf("%s(): Invalid interface\n", __func__);
	}

	memset(pool, 0, alloc_size);

	pool->id = poolID;
	pool->pri = pri;
	pool->config = *poolCfg;
	pool->interface = *interface;
	pool->memPoolId = -1;

	sprintf(pool->mem.dev_name, "VBMPool%d", poolID);

	vbm->pools[poolID] = (VBMPool *)pool;

	return 0;
}

int VBMDestroyPool(int poolID)
{
	if (poolID > NR_MAX_FS_GROUPS)
		return -1;

	VBM *vbm = VBMGetInstance();
	VBMPool *pool = vbm->pools[poolID];
	if (pool == NULL)
		return -1;

	uint8_t i, frame_type;
	IMPFrameInfo *frame = NULL;
	for (i = 0; i < pool->config.nr_buffers; i++) {
		frame = GET_POOL_FRAME(pool, i);
		//free ncu buf
		frame_type = ((IMPFrameInfoPriv*)(frame->priv))->frame_type;
		uint8_t *ncu_sta_y_buf = (uint8_t *)((IMPFrameInfoPriv*)(frame->priv))->ncuinfo.sta_y_buf;
		if (ncu_sta_y_buf) {
			free(ncu_sta_y_buf);
		}

		del_frame_from_volume(frame);
	}

	memset(pool, 0, sizeof(VBMPool));
	free(pool);
	vbm->pools[poolID] = NULL;

	return 0;
}

int VBMFlushFrame(int poolID)
{
	if (poolID > NR_MAX_FS_GROUPS)
		return -1;

	VBM *vbm = VBMGetInstance();
	VBMPool *pool = vbm->pools[poolID];
	if (pool == NULL)
		return -1;

	int i = 0;
	IMPFrameInfo *frame = NULL;
	FrameVolume *fvol = NULL;
	int ref = 0;
	int refSyncCnt = 0;

	for (i = 0; i < pool->config.nr_buffers; i++) {
		frame = GET_POOL_FRAME(pool, i);
		fvol = frame_to_vol(frame);
		if (fvol == NULL) {
			printf("frame:pool_idx=%d, index=%d, virAddr=%x can't find\n", frame->pool_idx, frame->index, fvol->ref, frame->virAddr);
			return -1;
		}

		do {
			pthread_mutex_lock(&fvol->mutex);
			ref = fvol->ref;
			pthread_mutex_unlock(&fvol->mutex);
		} while((ref > 0) && (refSyncCnt++ < 1000) && !usleep(100));

		if ((ref > 0) && (refSyncCnt >= 1000)) {
			printf("frame:pool_idx=%d, index=%d, ref=%d, virAddr=%x maybe deadlocked\n", frame->pool_idx, frame->index, fvol->ref, frame->virAddr);
			return -1;
		}
	}

	return 0;
}

int fsgetframeparam(int chn,IMPFrameInfo *pframe)
{
	if(chn < 0 || chn > 6){
		printf("chn out range,chn%d\n",chn);
		return -1;
	}
	if(NULL == pframe){
		printf("pframe is null,check!\n",chn);
		return -1;
	}
	VBM *vbm = VBMGetInstance();
	if (vbm->pools[chn] == NULL){
		printf("vbm is null\n");
		return -1;
	}

	IMPFrameInfo *frame = NULL;
	frame = GET_POOL_FRAME(vbm->pools[chn], 0);
	memcpy(pframe,frame,sizeof(IMPFrameInfo));
	return 0;
}

int VBMFillPool(int poolID)
{
	int ret = 0;
	VBM *vbm = VBMGetInstance();
	if (vbm->pools[poolID] == NULL)
		return -1;

	int i;
	IMPFrameInfo *frame = NULL;
	for (i = 0; i < vbm->pools[poolID]->config.nr_buffers; i++) {
		frame = GET_POOL_FRAME(vbm->pools[poolID], i);
		ret = vbm->pools[poolID]->interface.releaseFrame(frame, vbm->pools[poolID]->pri);
		if (ret < 0) {
			printf("%s:releaseFrame failed:frame->pool_idx=%d, frame->index=%d\n", __func__, frame->pool_idx, frame->index);
		}
	}

	return 0;
}

IMPFrameInfo *VBMGetFrameInstance(int poolID, int frame_index)
{
	VBM *vbm = VBMGetInstance();
	if (vbm->pools[poolID] == NULL)
		return NULL;

	return GET_POOL_FRAME(vbm->pools[poolID], frame_index);
}

int VBMFlushPool(int poolID)
{
	VBM *vbm = VBMGetInstance();
	if (vbm->pools[poolID] == NULL)
		return -1;

	int i;
	IMPFrameInfo frame;
	for (i = 0; i < vbm->pools[poolID]->config.nr_buffers; i++) {
		frame.pool_idx = poolID;
		vbm->pools[poolID]->interface.getFrame(&frame, vbm->pools[poolID]->pri);
	}

	return 0;
}

//extern int ncu_get_sta_data(int frame_num, void *data);
static int tiziano_ncu_get_sta_data(int frame_num, IMPNCUFrameInfo *framencuinfo, IMPISPNcuInfo *ispncuinfo)
{
	int i = 0;

	if ((NULL == framencuinfo) || (NULL == framencuinfo->sta_y_buf)) {
		printf("error(%s, %d): ptr data is null, frame_num = %d\n", __func__, __LINE__, frame_num);
		return -1;
	}

    IMPAlloc *pncualloc = IMP_ISP_Tuning_GetNCUAlloc();
    if (pncualloc == NULL) {
		printf("error(%s, %d): pncualloc is null, frame_num = %d\n", __func__, __LINE__, frame_num);
		return -1;
    }
	uint8_t *buf = (uint8_t*)pncualloc->info.vaddr;
	uint32_t size = ispncuinfo->sta_y_buf_size;
	uint32_t cnt = 20;
	uint32_t stride = 0;
	uint32_t tmpsize = 0, tsize = 0;
	stride = ((ispncuinfo->width+7)/8)*8;
	tmpsize = stride*ispncuinfo->height;
	tsize += tmpsize;

	stride = ((ispncuinfo->width+7)/8)*8;
	tmpsize = stride*ispncuinfo->height/2;
	tsize += tmpsize;

	stride = ((((ispncuinfo->width+15)/16)+7)/8)*8*2;
	tmpsize = stride*((ispncuinfo->height+15)/16+1);
	tmpsize *= 20;

	buf = (uint8_t*)pncualloc->info.vaddr + tsize;
	cnt = cnt <= tmpsize / size ? cnt : tmpsize / size;

	//printf("------------------------------------------------------------------------\n");
	for (i = 0; i < cnt; i++) {
        //printf("*(uint32_t*)(buf+size*%d)=%d, pncualloc->info.paddr + tsize + size * %d=0x%x\n", i, *(uint32_t*)(buf+size*i), i, pncualloc->info.paddr + tsize + size * i);
		if (*(uint32_t*)(buf+size*i) == frame_num) {
			//printf("--------%d---------\n", frame_num);
			uint8_t *sta_y_buf = buf+size*i+ispncuinfo->sta_y_stride;
			uint32_t sta_y_buf_size = size-ispncuinfo->sta_y_stride;
            memcpy(framencuinfo->sta_y_buf, sta_y_buf, sta_y_buf_size);
            framencuinfo->width = ispncuinfo->width;
            framencuinfo->height = ispncuinfo->height;
            framencuinfo->frm_num = frame_num;
            framencuinfo->sta_y_block_size = ispncuinfo->sta_y_block_size;
            framencuinfo->sta_y_stride = ispncuinfo->sta_y_stride;
			break;
		}
	}
	//printf("------------------------------------------------------------------------\n");
	if (i == cnt) {
		goto error;
	}
	return 0;
error:
	if (frame_num != 1) {
		printf("warn frm_num = %d\n", frame_num);
	}
	return -1;
}
int VBMGetFrame(int poolID, IMPFrameInfo **frame)
{
	VBM *vbm = VBMGetInstance();
	if (vbm->pools[poolID] == NULL)
		return -1;

	IMPFrameInfoPadPrivData tmp;
	tmp.frame.pool_idx = poolID;
	vbm->pools[poolID]->interface.getFrame(&tmp.frame, vbm->pools[poolID]->pri);
	*frame = GET_POOL_FRAME(vbm->pools[poolID], tmp.frame.index);
	(*frame)->timeStamp = tmp.frame.timeStamp;
	(*frame)->timeStamp_ivdc = tmp.frame.timeStamp_ivdc;
	((IMPFrameInfoPriv*)((*frame)->priv))->frame_type = tmp.priv.frame_type;
	((IMPFrameInfoPriv*)((*frame)->priv))->sensor_id = tmp.priv.sensor_id;
	((IMPFrameInfoPriv*)((*frame)->priv))->ivdc_overflow_cnt = tmp.priv.ivdc_overflow_cnt;
	((IMPFrameInfoPriv*)((*frame)->priv))->data_threshold = tmp.priv.data_threshold;
	vbm->is_close_ncu = 1;
    if ((poolID == 0) && ((vbm->is_close_ncu != 1) && (access("/tmp/closencuinfo", F_OK)))) {
		uint32_t frm_num = tmp.priv.ncuinfo.frm_num;
        IMPNCUFrameInfo *framencuinfo = &((IMPFrameInfoPriv*)((*frame)->priv))->ncuinfo;
        uint16_t *sta_y_buf = (uint16_t *)framencuinfo->sta_y_buf;
        IMPISPNcuInfo ispncuinfo;
        if (IMP_ISP_Tuning_GetNCUInfo(&ispncuinfo) == 0) {
            if (tiziano_ncu_get_sta_data(frm_num, framencuinfo, &ispncuinfo) < 0) {
				((IMPFrameInfoPriv*)((*frame)->priv))->b_use_ncuinfo = 0;
			}else {
				((IMPFrameInfoPriv*)((*frame)->priv))->b_use_ncuinfo = 1;
			}

            if ((!access("/tmp/dbgncuinfo", F_OK)) && (poolID == 0) && ((frm_num % 16) == 0)) {
                int i = 0;
                printf("-------------------\n");
                printf("frm_num = %d\n", frm_num);
                printf("width = %d\n", ispncuinfo.width);
                printf("height = %d\n", ispncuinfo.height);
                printf("sta_y_block_size = %x\n", ispncuinfo.sta_y_block_size);
                printf("sta_y_stride= %d\n", ispncuinfo.sta_y_stride);
                printf("sta_y_buf_size = %d\n", ispncuinfo.sta_y_buf_size);
                printf("sta_y_buf = %p\n", sta_y_buf);
                for (i = 0; i < ispncuinfo.sta_y_buf_size / 16; i++) {
					if ((i != 0) && (0 == i%(ispncuinfo.sta_y_stride/16))) {
						printf("\n");
					}
					printf("%08x", ((sta_y_buf[i * 8 + 0] & 0xf) << 28)
							| ((sta_y_buf[i * 8 + 1] & 0xf) << 24)
							| ((sta_y_buf[i * 8 + 2] & 0xf) << 20)
							| ((sta_y_buf[i * 8 + 3] & 0xf) << 16)
							| ((sta_y_buf[i * 8 + 4] & 0xf) << 12)
							| ((sta_y_buf[i * 8 + 5] & 0xf) << 8)
							| ((sta_y_buf[i * 8 + 6] & 0xf) << 4)
							| ((sta_y_buf[i * 8 + 7] & 0xf) << 0)
						  );
                }
            }
        }
	} else {
		((IMPFrameInfoPriv*)((*frame)->priv))->b_use_ncuinfo = 0;
	}
	return 0;
}

int VBMReleaseFrame(IMPFrameInfo *frame)
{
	return VBMUnLockFrame(frame);
}

void VBMDumpPool(int poolID)
{
	int i;
	VBM *vbm = VBMGetInstance();
	if (vbm->pools[poolID] == NULL)
		return;

	for (i = 0; i < vbm->pools[poolID]->config.nr_buffers; i++) {
		IMPFrameInfo *frame = GET_POOL_FRAME(vbm->pools[poolID], i);
		FrameVolume *frameVol = frame_to_vol(frame);
		printf("%s(%d):frame->pool_idx=%d, frame->index=%d, frameVol->ref=%d\n", __func__, __LINE__, frame->pool_idx, frame->index, frameVol->ref);
	}
}

void VBMDumpPoolInfo(void)
{
	int i,j;
	VBM *vbm = VBMGetInstance();
	for (i = 0; i < NR_MAX_FS_GROUPS; i++) {
		if (vbm->pools[i] == NULL)
			continue;
		printf("GROUP:%d\n",  i);
		for (j = 0; j < vbm->pools[i]->config.nr_buffers; j++) {
			IMPFrameInfo *frame = GET_POOL_FRAME(vbm->pools[i], j);
			IMPFrameInfoPriv *priv = (IMPFrameInfoPriv*)(frame->priv);
			printf("FRAME:%d  qcount:%d dqcount:%d\n",  frame->index, priv->qcount, priv->dqcount);
		}
	}
}
