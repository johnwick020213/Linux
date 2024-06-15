#ifndef __WD_FUNC_H
#define __WD_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <error.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <sys/uio.h>

#define SIZE(a) (sizeof(a)/sizeof(a[0]))

typedef void (*sighandler_t)(int);

#define ARGS_CHECK(argc, num)   {\
    if(argc != num){\
        fprintf(stderr, "ARGS ERROR!\n");\
        return -1;\
    }}

#define ERROR_CHECK(ret, num, msg) {\
    if(ret == num) {\
        perror(msg);\
        return -1;\
    }}

#define EVENT_ARR_SIZE 100
typedef struct train
{
    int len;
    char buff[1000];
}train_t;

typedef enum work_status
{
    FREE=0,
    BUSY
}work_status_t;

typedef struct
{
    pid_t pid;
    int pipefd;
    work_status_t status;
}process_data_t;

int makeChild(process_data_t*p,int num);
//pipefd是子进程和父进程通信用的
int handleTask(int pipefd);
int tcpInit(const char*ip,const char*port);

int epollAddFd(int epfd,int fd);
int epollDelFd(int epfd,int fd);

int sendFd(int pipefd,int fd);
int recvFd(int pipefd,int *pfd);

int transferFile(int peerfd);

#endif


