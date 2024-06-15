#include "process_pool.h"
#include<asm-generic/socket.h>

int makeChild(process_data_t *p,int num)
{
    for(int i=0;i<num;++i)
    {
        int pipefds[2];
        socketpair(AF_LOCAL,SOL_SOCKET,0,pipefds);
        pid_t pid=fork();
        if(pid==0)//子进程
        {
            close(pipefds[1]);
            handleTask(pipefds[0]);
            exit(0);
        }
        
        //父进程要保存子进程信息
        close(pipefds[0]);
        printf("child%d\n",pid);
        p[i].pid=pid;
        p[i].pipefd=pipefds[1];
        p[i].status=FREE;
        
    }
    return 0;
}

//该函数执行结束子进程退出
int handleTask(int pipefd)
{
    printf("child handle task.\n");
    while(1)
    {
        int peerfd;
        //当没有新的连接到来时，父进程不会调用sendFd函数
        //子进程会阻塞在recvF的函数上，等待新的连接到来
        printf("before recvFd.\n");
        int ret =recvFd(pipefd,&peerfd);
        printf("child peerfd:%d,ret:%d\n",peerfd,ret);

        //执行发送文件请求的操作
        transferFile(peerfd);
        //关闭连接（只有子进程关闭还不行）
        //父进程peerfd也要关闭
        //才能做到真正的关闭一个连接
        close(peerfd);
        //通知父进程
        int one=1;
        ret =write(pipefd,&one,sizeof(one));
    }
    return 0;
}

