cmake_minimum_required(VERSION 3.28)
enable_language(ASM)
project(OS_project C)
project(Bank C)
project(GTHREAD C)
project(Test C)

set(CMAKE_C_STANDARD 11)

add_executable(OS_project main.c)
add_executable(Bank "cmake-build-debug/client.c")
add_executable(GTREAD
        priority_queue.c
        priority_queue.h
        _gthread.c
        _gthread.h
        ctx_switch.S
        _gthread_context.c
        _gthread_context.h
        gthread.c
        gthread.h
        SJF.c
        SJF.h
        tester.c
        gthread_schedule.c
        gthread_schedule.h
        gthread_synchronization.c
        gthread_synchronization.h
        queue.c
        queue.h
        gthread_linkedlist.c
        gthread_linkedlist.h
        _gthread_runtime.c
        _gthread_runtime.h
        gthread_linkedlist_internal.h
        gthread_sched_internal.h
)
add_executable(Test
        test.c
        priority_queue.c
        priority_queue.h
        queue.c
        queue.h
        gthread_linkedlist.c
        gthread_linkedlist.h
        gthread_linkedlist_internal.h
)

#target_sources(GTREAD PRIVATE ctx_switch.S)

