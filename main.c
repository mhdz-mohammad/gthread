#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <printf.h>
#include <limits.h>
#include <termios.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>



#define p_num 2

#define SEND 1
#define PRINT 2
#define TERMINATE 3

struct command_struct
{
    int comno;
    int rsv_id;
    char msg[100];
};

int main()
{
    int processes_fd[p_num][2];
    pid_t processes[p_num];

    time_t start;
    time(&start);

    for (int i = 0; i < p_num; ++i) {
        if(pipe(processes_fd[i]) == -1)
            exit(-1);
    }
    for (int i = 0; i < p_num; ++i) {
        switch (processes[i] = fork()) {
            case -1:
                exit(-1);
            case 0: //child process
                char text[100];
                sprintf(text, "process result of %d", i);
                FILE *res = fopen(text, "w+");
                if(res == NULL)
                {
                    printf("couldn't open the associated file\n");
                    exit(-1);
                }
                for (int j = 0; j < p_num; ++j) {
                    if(j == i) continue;
                    if(close(processes_fd[j][0]) == -1) //deleting unused read
                        exit(-1);
                }
                if (close(processes_fd[i][1])) //closing unused write end
                    exit(-1);

                struct command_struct snd_cmd, rcv_cmd;
                while(read(processes_fd[i][0], &rcv_cmd, sizeof(rcv_cmd)) == sizeof(rcv_cmd))
                {
                    switch (rcv_cmd.comno) {
                        case SEND:
                            fprintf(res, "got instruction with rcv_id = %d, msg = %s\n", rcv_cmd.rsv_id, rcv_cmd.msg);
                            snd_cmd.comno = PRINT;
                            strcpy(snd_cmd.msg, rcv_cmd.msg);

                            write(processes_fd[rcv_cmd.rsv_id][1], &snd_cmd, sizeof(snd_cmd));
                            break;
                        case PRINT:
                            fprintf(res, "I (i = %d, pid = %d) received a msg as following %s\n", i, getpid(), rcv_cmd.msg);
                            break;
                        case TERMINATE:
                            time_t end;
                            time(&end);
                            fprintf(res, "got terminate instruction\n ended at %lf", difftime(end, start));
                            fclose(res);
                            return 0;

                    }
                }
                fclose(res);
                return 0;
            default:
                break;
        }
    }

    for (int i = 0; i < p_num; ++i) {
        if (close(processes_fd[i][0]) == -1)
            exit(-1);
    }
    char request[40];
    char arg[4][50];

    scanf("%s", request);
    while(strcmp(request, "finish") != 0)
    {
        if (strcmp(request, "communicate") == 0)
        {
            printf("enter the sender and receiver id: \n");
            scanf(" %s %s %s", arg[0], arg[1], arg[2]);

            struct command_struct tmp;
            int snd_id = (int)strtol(arg[0], NULL, 10);
            tmp.comno = SEND;
            tmp.rsv_id = (int)strtol(arg[1], NULL, 10);
            strcpy(tmp.msg, arg[2]);

            write(processes_fd[snd_id][1], &tmp, sizeof(tmp));

        }
        scanf("%s", request);
    }

    struct command_struct tmp;
    tmp.comno = TERMINATE;
    for (int i = 0; i < p_num; ++i) {
        write(processes_fd[i][1], &tmp, sizeof(tmp));
    }
    for (int i = 0; i < p_num; ++i) {
        wait(NULL);
    }



    return 0;
}

