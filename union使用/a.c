#include<stdio.h>
typedef long long int int32;

typedef union {
	int rst_gpio;
	struct {
		int a : 1;
		int b : 1;
		int c : 1;
		int d : 1;
		int e : 12;
	};

} IMPSensorInfo;

int main()
{
#if 1
	IMPSensorInfo si;
        si.rst_gpio = 0x0;
        //si.a = 1;
        si.b = 1;
        si.c = 1;
        si.d = 1;

        printf("%d,%d,%d,%d,0x%x\n",si.a, si.b, si.c, si.d, si.rst_gpio);
        if(si.a)
                printf("-- [%s,%d] --\n",__func__,__LINE__);


	printf("%ld\n", sizeof(si));
	printf("%ld\n", sizeof(int32));
	printf("%ld\n", sizeof(int));
	printf("%p\n", &si);
#endif

}

