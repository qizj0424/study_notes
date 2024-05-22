#include<signal.h>
#include<unistd.h>
#include <fcntl.h>
#include<stdio.h>
#include <string.h>
#include <stdlib.h>

int num = 0;

static inline void __signal_handler(int n, siginfo_t *info, void *ucontext)
{
        printf("------------------->[%d] sig = %d,data =  %d\n",num, n, info->si_int);
        if(n == SIGUSR1){
                if(info->si_int == 1)
                        printf("card inserted !!!\n");
                if(info->si_int == 0)
                        printf("card removed !!!\n");
                if(info->si_int == 3)
                        printf("simulation operation card inserted !!!\n");
                if(info->si_int == 2)
                        printf("simulation operation card removed !!!\n");
        }
        num++;

        // 重置信号处理，否则循环进入.
        //signal(n, SIG_DFL);
}

void regist_signal()
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));

    sigemptyset(&action.sa_mask);
    action.sa_sigaction = __signal_handler;
    action.sa_flags = SA_SIGINFO;

#if 0
    // Use the alternate signal stack if available so we can catch stack overflows.
    action.sa_flags |= SA_ONSTACK;

    sigaction(SIGABRT, &action, NULL);
    sigaction(SIGBUS, &action, NULL);
    sigaction(SIGFPE, &action, NULL);
    sigaction(SIGILL, &action, NULL);
    sigaction(SIGPIPE, &action, NULL);
    sigaction(SIGSEGV, &action, NULL);
    sigaction(SIGTRAP, &action, NULL);
#endif
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);
    printf("SIGUSR2 is %d,SIGUSR1 = %d\n",SIGUSR1,SIGUSR2);
}

int main(int argc, char **argv)
{
	int fd;
        int g_pid = 0;

        regist_signal();
        g_pid = getpid();
        printf("This pid is %d\n",g_pid);


	fd = open("/dev/hello", O_RDWR);
	if (fd == -1){
		printf("can not open file /dev/hello\n");
		return -1;
	}

        write(fd, (void *)&g_pid, sizeof(int));


        while(1){
                char input[100];  // 用于存储用户输入的字符串
                int num = 0;

                printf("请输入:");
                scanf("%99s", input);  // 从终端读取输入，不包括空白字符
                if(!strcmp(input,"exit")){
                        goto free;
                }else{
                        num = atoi(input);
                        write(fd, (void *)&num, sizeof(int));
                }

        }

free:
        g_pid = 0xFFFF;
        write(fd, (void *)&g_pid, sizeof(int));
	close(fd);
        return 0;
}

