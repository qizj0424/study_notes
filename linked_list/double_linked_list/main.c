#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> // 提供 container_of 宏
#include "double_linked_list.h"

#define LIST_DEPTH 10
struct list_head Node_list;

struct mydata {
        int data;
        struct list_head entry;
};

void prt_list()
{

        printf("-- printf list --\n");
        struct mydata *pos;
        list_for_each_entry(pos, &Node_list, entry)
                printf("date = %d\n",pos->data);

}

int main()
{
        struct mydata *test = malloc(sizeof(struct mydata) * LIST_DEPTH);
        struct mydata *pos, *n_pos;
        int i = 0;
        // 初始化空链表
        INIT_LIST_HEAD(&Node_list);
        prt_list();//遍历整个链表

        for (i = 0; i < 5;i++){
                test[i].data = i;
                list_add_tail(&test[i].entry, &Node_list); //尾部添加
        }
        prt_list();//遍历整个链表

        //遍历链表
        list_for_each_entry_safe(pos, n_pos, &Node_list, entry){
                if(pos->data == 3)//找到目标 删除
                        list_del(&pos->entry);
                if(pos->data == 2)//找到目标修改
                        pos->data = 22;
        }
        prt_list();//遍历整个链表

        test[6].data = 6;
        list_add(&test[6].entry, &Node_list); //插入链表头
        prt_list();//遍历整个链表

        //清空链表
        list_for_each_entry_safe(pos, n_pos, &Node_list, entry){
                list_del(&pos->entry);
        }
        prt_list();//遍历整个链表

        free(test);

        return 0;
}




