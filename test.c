//
// Created by Mohammad Bagher Mhdz on 2/2/2026 A.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdatomic.h>
#include <pthread.h>
#include "gthread_linkedlist.h"

typedef struct custom
{
    int a;
    char name[100];
}s_t;

_Atomic int n = 0;
int cnt = 0;

int compare(uintptr_t l, uintptr_t r)
{
    s_t *sl = (s_t*)l;
    s_t *sr = (s_t*)r;

    if(sl->a >= sr->a) return 0;
    else return -1;
}
void* f(void *arg)
{
    for (int i = 0; i < 1000; ++i) {
        atomic_fetch_add(&n, 1);
        cnt += 1;
    }

    return NULL;
}
int main()
{
    pthread_t threads[10];
    for (int i = 0; i < 10; ++i) {
        pthread_create(threads + i, NULL, f, NULL);
    }
    for (int i = 0; i < 10; ++i) {
        pthread_join(threads[i], NULL);
    }
    printf("atomic number: %d, non atomic number: %d", n, cnt);
    return 0;
}