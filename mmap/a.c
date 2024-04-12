#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BASE_ADDR 0x02000000
#define MMAP_SIZE 4685824

int main(void)
{

        int fd;
        void *map_addr;
        //打开/dev/mem设备文件
        if((fd = open("/dev/mem",O_RDWR |O_SYNC))== -1){
                perror("open");
                exit(EXIT_FAILURE);
        }

        map_addr = mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BASE_ADDR);
        if(map_addr == MAP_FAILED){
                perror("mmap");
                close(fd);
                exit(EXIT_FAILURE);
        }
        //写数据
        //memset(map_addr, 0xaa, MMAP_SIZE);

        //连续保存5次
        for(int i = 0;i < 5;i++){
                //创建并打开文件
                char filename[20];
                sprintf(filename, "snap-open-mdns-%d.bin", i);
                FILE *file = fopen(filename, "wb+");
                if (!file){
                        perror("fopen");
                        munmap(map_addr, MMAP_SIZE);
                        close(fd);
                        exit(EXIT_FAILURE);
                }
                //将映射的内存写入文件
                fwrite(map_addr, MMAP_SIZE ,1, file);
                fclose(file);
                sleep(1);
        }
        return 0;
}
