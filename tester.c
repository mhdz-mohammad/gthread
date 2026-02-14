//
// Created by Mohammad Bagher Mhdz on 2/3/2026 A.
//
#include "gthread.h"
#include "SJF.h"
#include <string.h>
#include <unistd.h>
#include <pthread_impl.h>

typedef struct custom{
    char * str;
    uint32_t len;
}custom;

void t1(void *arg)
{
    custom *c = (custom *)arg;
    gthread_block_sync_stdout(c->str, c->len);
}
char *str1 = "first string \n";
char *str2 = "second string \n";
custom c1, c2;
int main()
{
    c1.str = str1; c1.len = strlen(str1);
    c2.str = str2; c2.len = strlen(str2);
    gt_usr_sched_t *gtUsrSched;
    gthread_sjf_create(&gtUsrSched);
    gthread_pool_t *gthreadPool;
    gthread_create(&gthreadPool, 1, gtUsrSched);
    gthread_add_task(gthreadPool, t1, (void*)&c1);
    gthread_add_task(gthreadPool, t1, (void*)&c2);

    gthread_join(gthreadPool);
    return 0;
}