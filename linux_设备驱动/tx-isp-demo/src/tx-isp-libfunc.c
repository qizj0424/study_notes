#include <linux/slab.h>
#include <tx-isp-libfunc.h>

void *private_kmalloc(size_t size, gfp_t flags)
{
        void *ptr = kmalloc(size, flags);
        //printk("-- [%s,%d] alloc = %p --\n",__func__,__LINE__,ptr);
        return ptr;
}

void private_kfree(const void *ptr)
{
        //printk("-- [%s,%d] free = %p --\n",__func__,__LINE__,ptr);
        kfree(ptr);
}

