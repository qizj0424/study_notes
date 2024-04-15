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

typedef struct {
	int rst_gpio;
        int a;
        int b;
        int c;
} IMPSensorInfo;

typedef struct {
        int cc;
        IMPSensorInfo sensor;
} Testfile1;

int main(void)
{

        int fd;
        Testfile1 *test = NULL;
        //打开/dev/mem设备文件
        if((fd = open("/dev/mem",O_RDWR |O_SYNC))== -1){
                perror("open");
                exit(EXIT_FAILURE);
        }

        test = mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, BASE_ADDR);
        if(test == MAP_FAILED){
                perror("mmap");
                close(fd);
                exit(EXIT_FAILURE);
        }

        printf("--> a = %d, b = %d,c = %d\n", test->sensor.a, test->sensor.b, test->sensor.c);
        close(fd);
        return 0;
}
