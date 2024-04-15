#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BASE_ADDR 0x3c00000
#define MMAP_SIZE 20

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
        FILE *file = fopen("test.bin", "r+");
        fread(map_addr, MMAP_SIZE, 1, file);
        fclose(file);

        close(fd);
        return 0;
}
