//FIFO.h文件
 
#ifndef _FIFO_H_
#define _FIFO_H_
 
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
 
/**
* @brief 创建fifo
* @param item_sum：fifo中元素数.注意不是字节数
* @param item_size: 元素大小.单位: 字节
* @return fifo索引
*/
 
extern int fifo_create(int item_sum, int item_size);
 
/**
* @brief 删除fifo
* @param fifo_index: fifo索引
*/
 
extern void fifo_delete(int fifo_index);
 
/**
* @brief fifo检查是否可以写入
* @param fifo_index: fifo索引
* @retval false:不可以写入.true:可以写入
*/
 
extern bool fifo_writeable(int fifo_index);
 
/**
* @brief fifo写入
* @param fifo_index: fifo索引
* @param frame:写入元素指针
* @return false:失败.true:成功
*/
 
extern bool fifo_write(int fifo_index, void *data);
 
/**
* @brief fifo批量写入
* @param fifo_index: fifo索引
* @param data: 写入元素指针
* @param @param item_num：写入的元素数目
* @return false:失败.true:成功
*/
 
extern bool fifo_write_batch(int fifo_index, void *data, int item_num);
 
/**
* @brief fifo检查是否可以读取
* @param fifo_index: fifo索引
* @return false:不可以读取.true:可以读取
*/
 
extern bool fifo_readable(int fifo_index);
 
/**
* @brief fifo读取
* @param fifo_index: fifo索引
* @param data: 读取的数据
* @return false: 失败.true: 成功
*/
 
extern bool fifo_read(int fifo_index, void *data);
 
/**
* @brief fifo批量读取
* @param fifo_index: fifo索引
* @param data: 读取的数据
* @param item_num：读取的元素数
* @return false: 失败.true: 成功
*/
 
extern bool fifo_read_batch(int fifo_index, void *data, int item_num);
 
/**
* @brief fifo可读的元素数
* @param fifo_index: fifo索引
* @return 元素数
*/
 
extern int fifo_readable_item_count(int fifo_index);
 
/**
* @brief fifo可写的元素数
* @param fifo_index: fifo索引
* @return 元素数
*/
 
extern int fifo_writeable_item_count(int fifo_index);
 
 
extern void TestFifo(void);
#endif
