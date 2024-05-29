#ifndef __TX_ISP_LIBFUNC_H__
#define __TX_ISP_LIBFUNC_H__


void *private_kmalloc(size_t size, gfp_t flags);
void private_kfree(const void *ptr);


#endif
