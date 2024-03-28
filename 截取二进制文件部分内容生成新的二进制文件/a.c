
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE (1920*1080)

int main() {
    FILE *input_file, *output_file;
    unsigned char buffer[BUFFER_SIZE * 3/2];

    // 打开输入文件
    input_file = fopen("4-all.yuv", "rb");
    if (input_file == NULL) {
        perror("无法打开输入文件");
        return 1;
    }

    // 打开输出文件
    output_file = fopen("test.yuv", "wb");
    if (output_file == NULL) {
        perror("无法创建输出文件");
        fclose(input_file);
        return 1;
    }

    // 读取输入文件的内容
    fseek(input_file, 0, SEEK_END);
    long file_size = ftell(input_file);
    rewind(input_file);

#if 0
    if (file_size < BUFFER_SIZE * 2) {
        perror("输入文件的内容不足");
        fclose(input_file);
        fclose(output_file);
        return 1;
    }
#endif

    // 读取并保存前1080个字节的内容
    size_t bytes_read_first = fread(buffer, sizeof(unsigned char), BUFFER_SIZE, input_file);

    // 将文件指针移动到倒数第1080个字节位置
    fseek(input_file, -(1920*1080/2), SEEK_END);

    // 读取并保存最后1080个字节的内容
    size_t bytes_read_last = fread(buffer + BUFFER_SIZE, sizeof(unsigned char), BUFFER_SIZE/2, input_file);

    // 将读取到的内容写入输出文件
    size_t bytes_written = fwrite(buffer, sizeof(unsigned char), BUFFER_SIZE*3/2, output_file);

    // 关闭文件
    fclose(input_file);
    fclose(output_file);

    printf("文件生成成功。\n");
    return 0;
}
