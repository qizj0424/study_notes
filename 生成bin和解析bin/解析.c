#include<stdio.h>
#include <sys/fcntl.h>

typedef long long int int32;

typedef struct {
	int rst_gpio;
        int a;
        int b;
        int c;
} IMPSensorInfo;

typedef struct {
        int cc;
        IMPSensorInfo sensor;
} Testfile;

int main()
{
	char file_name[64];
	FILE *fp = NULL;

        Testfile binfile;

        snprintf(file_name, sizeof(file_name), "%s.bin", "test");

	fp = fopen(file_name,"r+");
	if(fp == NULL){
		printf("Open in file dump failed!!!\n");
		return -1;
	}
	fread(&binfile,sizeof(binfile),1,fp);
	fclose(fp);
        printf("%d, %d, %d, %d, %d\n",binfile.cc, binfile.sensor.rst_gpio, binfile.sensor.a, binfile.sensor.b, binfile.sensor.c);
        return 0;
}

