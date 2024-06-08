#include<func.h>
#include<sys/epoll.h>

#define CONNS_MAX 1024

typedef struct conn_s
{
    int peerfd;//保存与对端进行交互的peerfd
    int lastTime;//保存该连接上一次通信的事间
}conn_info_t;

int main()
{
    //创建监听套接字
    int listenfd =socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(listenfd,-1,"socket");
    printf("listenfd:%d\n",listenfd);

    struct sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(8080);
    serveraddr.sin_addr.s_addr=inet_addr("192.168.127.129");

    //设置套接字属性，网络地址可重用
    int on=1;
    int ret=setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(int));
    ERROR_CHECK(ret,-1,"setsockopt");

    //绑定网络地址
    ret=bind(listenfd,(const struct sockaddr*)&serveraddr,sizeof(serveraddr));
    ERROR_CHECK(ret,-1,"bind");

    //进行监听
    ret =listen(listenfd,10);
    ERROR_CHECK(ret,-1,"listen");
    printf("服务器开始监听。\n");

    //创建epoll实例
    int epfd =epoll_create1(0);
    ERROR_CHECK(epfd,-1,"epoll_create1");

    //对相应的文件描述符进行监听
    struct epoll_event ev;
    ev.events=EPOLLIN;
    ev.data.fd=listenfd;
    ret =epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
    ERROR_CHECK(ret,-1,"epoll_ctl");
    struct epoll_event*pEventArr=(struct epoll_event*)calloc(CONNS_MAX,sizeof(struct epoll_event));

    //数组来保存已经建立好的连接
    conn_info_t conns[CONNS_MAX]={0};

    //执行事件循环
    while(1)
    {
        int nready=epoll_wait(epfd,pEventArr,CONNS_MAX,1000);
        if(nready==-1&&errno==EINTR)
        {
            continue;
        }
        else if(nready==0)
        {
            
            //超时处理
            int curTime=time(NULL);
            //遍历conns数组，从中查找是否有超时连接
            for(int i=0;i<CONNS_MAX;++i)
            {
                if(conns[i].peerfd>0&&curTime-conns[i].lastTime>=30)
                {
                    //从监听红黑树上删除
                    struct epoll_event ev;
                    ev.data.fd=conns[i].peerfd;
                    int ret =epoll_ctl(epfd,EPOLL_CTL_DEL,conns[i].peerfd,&ev);
                    ERROR_CHECK(ret,-1,"epoll_ctl");
                    //关闭连接
                   close(conns[i].peerfd);
                   printf("用户%d已超时。\n",conns[i].peerfd);
                   //清空peerfd的记录
                   conns[i].peerfd=0;
                   conns[i].lastTime=0;
                }
            }
        }
        else if(nready==-1)
        {
            break;
        }
        else
        {
            //nready>0的情况
            //对nready个文件描述符进行处理
            for(int i=0;i<nready;++i)
            {
                int fd=pEventArr[i].data.fd;
                //listenfd的处理，建立新的连接
                if(fd==listenfd)
                {
                    struct sockaddr_in clientAddr;
                    memset(&clientAddr,0,sizeof(clientAddr));
                    socklen_t len=sizeof(clientAddr);
                    int peerfd=accept(listenfd,(struct sockaddr*)&clientAddr,&len);
                    printf("%s:%d 已经连接，peerfd:%d.\n",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port),peerfd);

                    struct epoll_event ev;
                    ev.events=EPOLLIN;
                    ev.data.fd=peerfd;
                    ret =epoll_ctl(epfd,EPOLL_CTL_ADD,peerfd,&ev);
                    //保存该连接的信息，放如conns，并记录第一次交互的时间
                    for(int i=0;i<CONNS_MAX;++i)
                    {
                        if(conns[i].peerfd==0)
                        {
                            conns[i].peerfd=peerfd;
                            conns[i].lastTime=time(NULL);
                            break;
                        }
                    }
                }
                else
                {
                    //对peerfd的处理
                    //判断是否发生读事件
                    if(pEventArr[i].events&EPOLLIN)
                    {
                        char buff[1024]={0};
                        int ret =recv(fd,buff,sizeof(buff),0);
                        if(ret==0)
                        {
                            //连接断开的情况
                            ev.data.fd=fd;//从监听红黑树删除
                            ret=epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&ev);
                            ERROR_CHECK(ret,-1,"epoll_ctl");
                            //更新conns中的信息
                            for(int i=0;i<CONNS_MAX;++i)
                            {
                                if(conns[i].peerfd==fd)
                                {
                                    conns[i].peerfd=0;
                                    conns[i].lastTime=0;
                                    break;
                                }
                            }
                            close(fd);
                            printf("再见。\n");
                            continue;
                        }
                        printf("ret:%d，信息：%s\n",ret,buff);
                        //记录交互最新的时间信息
                        for(int i=0;i<CONNS_MAX;++i)
                        {
                            //在conns中查找fd所在的位置
                            if(conns[i].peerfd==fd)
                            {
                                conns[i].lastTime=time(NULL);
                                break;
                            }
                        }

                        //执行转发操作
                        for(int j=0;j<CONNS_MAX;++j)
                        {
                            if(conns[j].peerfd>0&&conns[j].peerfd!=fd)
                            {
                                send(conns[j].peerfd,buff,strlen(buff),0);
                            }
                        }
                    }
                }
            }
        }
    }
    close(listenfd);
    close(epfd);

    return 0;
}
