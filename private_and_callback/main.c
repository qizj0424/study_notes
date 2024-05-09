#include <stdio.h>
#include "main.h"

int callbak(struct AAA *test1)
{
        printf("%d,%d,%d,%d\n",test1->a,test1->b,test1->c,test1->pri[0]);

        return 0;
}

int main()
{
        init(callbak);

        return 0;

}
