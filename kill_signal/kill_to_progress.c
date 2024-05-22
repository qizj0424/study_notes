#include<signal.h>
#include<unistd.h>
#include <fcntl.h>
#include<stdio.h>
#include <string.h>

int num = 0;
static inline void __signal_handler(int n, siginfo_t *info, void *ucontext)
{

        printf("------------------->[%d] sig = %d,data =  %d\n",num, n, info->si_int);
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

        regist_signal();

        while(1);

        return 0;
}

