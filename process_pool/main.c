#include "process_pool.h"

int main(int argc,char**argv)
{
    ARGS_CHECK(argc,4);
    int processNum=atoi(argv[3]);
    //创建存储进程池信息的结构体
    process_data_t *pProcess=(process_data_t *)calloc(processNum,sizeof(process_data_t));
    //创建N个子进程
    makeChild(pProcess,processNum);
    
    //创建监听套接字，并等待客户端的到来
    int listenfd =tcpInit(argv[1],argv[2]);

    //创建epoll的实例
    int epfd=epoll_create1(0);
    ERROR_CHECK(epfd,-1,"epoll_create1");
    //用epoll监听listenfd
    epollAddFd(epfd,listenfd);
    for(int i=0;i<processNum;++i)
    {
        epollAddFd(epfd,pProcess[i].pipefd);
    }
    
    struct epoll_event *pEventArr=(struct epoll_event*)calloc(EVENT_ARR_SIZE,sizeof(struct epoll_event));

    //执行epoll的事件循环操作
    while(1)
    {
        int nready=epoll_wait(epfd,pEventArr,EVENT_ARR_SIZE,5000);

        for(int i=0;i<nready;++i)
        {
            int fd=pEventArr[i].data.fd;
            if(fd==listenfd)//客户端有新的连接
            {
                int peerfd=accept(listenfd,NULL,NULL);
                printf("parent peerfd:%d\n",peerfd);

                //查找一个空闲的子进程去执行任务
                for(int j=0;j<processNum;++j)
                {
                    if(pProcess[j].status==FREE)
                    {
                        //将peerfd装交给子进程进行处理
                        sendFd(pProcess[j].pipefd,peerfd);
                        pProcess[j].status=BUSY;
                        break;
                    }
                }
                //必须关闭peerfd
                close(peerfd);
            }
            else
            {
                int howmany=0;
                read(fd,&howmany,sizeof(howmany));
                for(int j=0;j<processNum;++j)
                {
                    if(pProcess[j].pipefd==fd)
                    {
                        pProcess[j].status=FREE;
                        printf("child %d is not busy.\n",pProcess[j].pid);
                        break;
                    }      
                }
            }
        }
    }
    return 0;
}

