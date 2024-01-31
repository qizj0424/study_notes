#include <stdio.h>

int main(void)
{
    char *p1 = (char *)malloc(sizeof(char));
    char **p2 = &p1;
    int a[10];
    int *b = NULL;
    int i;

    for(i = 0; i < 10; i++)
        a[i] = i*3;

    b =  a;
    
    for(i = 0; i < 10; i++)
    {
        printf("%d\n",*a);
        (a)++;
    }
        printf("%d\n\n\n",*b);
    
    for(i = 0; i < 10; i++)
    {
        printf("%d\n",*b);
        b++;
    }

    memset(p1,1,sizeof(char));
    printf("%d\n",*p1);
    printf("%d\n",**p2);
    printf("0x%08x\n",p1);
    printf("0x%08x\n",*p2);

    memset(p1,2,sizeof(char));
    printf("%d\n",*p1);
    printf("%d\n",**p2);
    printf("0x%08x\n",p1);
    printf("0x%08x\n",*p2);

    return 0;
}
