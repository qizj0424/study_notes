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
        binfile.cc = 16;
        binfile.sensor.rst_gpio = 9;
        binfile.sensor.a = 9;
        binfile.sensor.b = 8;
        binfile.sensor.c = 7;

        snprintf(file_name, sizeof(file_name), "%s.bin", "test");

	fp = fopen(file_name,"w+");
	if(fp == NULL){
		printf("Open in file dump failed!!!\n");
		return -1;
	}
	fwrite(&binfile,sizeof(binfile),1,fp);
	fflush(fp);
	fclose(fp);
        return 0;
}

