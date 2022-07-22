#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include<signal.h>
#include<sys/signal.h>

void ctrl_c_sighandler(int signum)
{
    printf("CTRL + C \n");
}

int main(int argc, char **argv)
{
    signal(SIGINT, ctrl_c_sighandler);

    while(1){
        sleep(1);
    }   
    return 0;
}