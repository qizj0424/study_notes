#ifndef __IMP_FUNCS_H__
#define __IMP_FUNCS_H__

enum debug_module {
	FUNC_MODULE_ISP,
	FUNC_MODULE_FS,
	FUNC_MODULE_ENC,
	FUNC_MODULE_MISC,
	FUNC_MODULE_AI_DEV,
	FUNC_MODULE_AO_DEV,
	FUNC_MODULE_END
};

/* enable server dbg print */
#define FUNC_FLAG_DBG (0x1<<0)
/* get next data of a func from server */
#define FUNC_FLAG_NEXT_DATA (0x1<<1)
/* data to client is end */
#define FUNC_FLAG_END (0x1<<8)

#define FUNC_OUT_DATA_TYPE_STR 0
#define FUNC_OUT_DATA_TYPE_UDF 1

typedef struct {
	void *buf;
	int size;
	int dtype;
	int is_end;
	void *shm_buf;
}umem_info;

/* format of buffer */
typedef struct {
	int mid;
	int fid;
	int flg;
	union {
		struct {
			int size;
			int udf[0];
		}in;
		struct {
			int ret;
			int type;
			int size;
			int udf[0];
		}out;
	};
} dformat;

int DsystemInit(void);
int DsystemExit(void);
int dsys_func_share_mem_register(int mid, int fid, char *name, int (*cb_smem)(void *buf, int msize, void **pri));
int dsys_func_user_mem_register(int mid, int fid, char *name, int (*cb_umem)(umem_info *uinfo, void **pri), int (*cb_umem_end)(void **pri));
int dsys_func_unregister(int mid, int fid);

#endif
