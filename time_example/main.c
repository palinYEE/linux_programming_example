#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>

#include<time.h>
#include<sys/time.h>
#include<signal.h>

static void print_time(void)
{
    time_t now; 
    struct tm now_tm;

    if(time(&now) == -1){
        printf("time() fail: %s\n", strerror(errno));
        return ;
    }

    if(localtime_r((const time_t *)&now, &now_tm) == NULL){
        printf("localtime_r() fail: %s\n", strerror(errno));
        return ;
    }

    printf("[TIME] %d-%d-%d %d:%d:%d\n", 1900+now_tm.tm_year, 1+now_tm.tm_mon, now_tm.tm_mday, now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec);

}

static void sigalarm_handler(int signum)
{
    printf("got SIGALRM(%d)\n", signum);
    print_time();
}

static void setting_itimerval(struct itimerval *val)
{
    val->it_interval.tv_sec = 1;
    val->it_interval.tv_usec = 0;
    val->it_value.tv_sec = 1;
    val->it_value.tv_usec = 0;
}

int main(int argc, char **argv)
{
    int ret = 0;
    struct itimerval val;
    /* 알람 시그널 등록 */
    signal(SIGALRM, (sig_t)sigalarm_handler);

    setting_itimerval(&val);
    ret = setitimer(ITIMER_REAL, (const struct itimerval *)&val, NULL);
    if(ret){
        printf("setitimer() fail: %s\n", strerror(errno));
        return -1;
    }
    printf("= Start infinite loop while(1)\n");
    while(1){
        sleep(1);
    }
    return 0;
    
}