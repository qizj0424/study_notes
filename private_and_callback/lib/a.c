#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../main.h"
char name[10] = "sss";

struct BBB {
        char ee[10];
        int dd;
        int ff;

};

int fun1(struct AAA *test1)
{
        printf("%d,%d,%d,%d,%d,%s\n",test1->a,test1->b,test1->c,
                        test1->pri[0],((struct BBB *)(test1->pri))->dd,((struct BBB *)(test1->pri))->ee);
        return 0;
}

int init(CallbackFunction cb)
{
        struct AAA *test = (struct AAA *)malloc(sizeof(struct AAA) + sizeof(struct BBB));
        test->a = 0;
        test->b = 1;
        test->c = 2;

        ((struct BBB *)(test->pri))->dd = 10;
        ((struct BBB *)(test->pri))->ff = 11;

        memcpy(((struct BBB *)(test->pri))->ee,name,sizeof(name)) ;

        fun1(test);
        cb(test);

        free(test);
        return 0;
}
