#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

volatile int nr_sigusr1 = 0;
sigset_t ms;
sigset_t msgol;
sigset_t mscopil;

void handlersigint(int signo)
{
    printf("Am primit %d\n", nr_sigusr1);
    exit(0);
}

void handlersigu1(int signo)
{
    nr_sigusr1++;
}

void handlersigu2(int signo)
{
}

int main()
{
    int pid;
    int nr_sigusr1_son;
    int nr_sigusr1_father;
    int i;
    int status;
    signal(SIGUSR1, handlersigu1);
    signal(SIGINT, handlersigint);
    sigemptyset(&ms);
    sigemptyset(&msgol);
    sigaddset(&ms, SIGUSR1);
    sigaddset(&ms, SIGUSR2);
    sigprocmask(SIG_SETMASK, &ms, NULL);
    pid = fork();

    if (pid == 0)
    {
        signal(SIGUSR2, handlersigu2);
        sigfillset(&mscopil);
        sigdelset(&mscopil, SIGUSR2);
        nr_sigusr1_son = rand() % 10 + 1;
        // nr_sigusr1_son = 5;
        for (i = 0; i < nr_sigusr1_son; i++)
        {
            kill(getppid(), SIGUSR1);
            sigsuspend(&mscopil);
            // sleep(1);
        }
        printf("Fiu a trimis %d semnale\n", nr_sigusr1_son);
    }
    else
    {
        // sigprocmask(SIG_UNBLOCK, &ms, NULL);
        sigfillset(&msgol);
        sigdelset(&msgol, SIGUSR1);
        sigdelset(&msgol, SIGINT);
        while (1)
        {
            sigsuspend(&msgol);
            kill(pid, SIGUSR2);
        }
        wait(&status);
    }

    return 0;
}