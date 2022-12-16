#include<unistd.h>
#include<stdio.h>
#include<signal.h>
#include<stdlib.h>

void handler(int signo)
{
    psignal(signo, "Received Signal: ");
    sleep(5);
    printf("In Signal Handler, After Sleep\n");
}

int main(void)
{
    struct sigaction act;

    sigemptyset(&act.sa_mask);          /* 시그널 처리 중 블록될 시그널은 없음. */
    sigaddset(&act.sa_mask, SIGQUIT);   
    act.sa_flags = 0;                   
    act.sa_handler = handler;           /* 시그널 핸들러 지정 */

    if(sigaction(SIGINT, &act, (struct sigaction *)NULL) < 0)
    {
        perror("sigaction");
        exit(1);
    }

    fprintf(stderr, "Input SIGINT: ");
    pause();
    fprintf(stderr, "After Signal Handler\n");

    return 0;
}