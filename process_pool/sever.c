#include"process_pool.h"
#include<sys/epoll.h>

int tcpInit(const char* ip,const char* port)
{
    //创建监听套接字
    int listenfd=socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(listenfd,-1,"socket");

    struct sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(atoi(port));
    serveraddr.sin_addr.s_addr=inet_addr(ip);
    
    //设置网络地址可重用
    int on=1;
    int ret=setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
    ERROR_CHECK(ret,-1,"setsockopt");
    
    //绑定网络地址
    ret=bind(listenfd,(const struct sockaddr*)&serveraddr,sizeof(serveraddr));
    ERROR_CHECK(ret,-1,"bind");

    //进行监听
    ret =listen(listenfd,10);
    ERROR_CHECK(ret,-1,"listen");
    return listenfd;
}

int epollAddFd(int epfd,int fd)
{
    struct epoll_event ev;
    memset(&ev,0,sizeof(ev));
    ev.events=EPOLLIN;
    ev.data.fd=fd;
    int ret=epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);
    ERROR_CHECK(ret,-1,"epoll_ctl");
    return 0;
}

int epollDelFd(int epfd,int fd)
{
    struct epoll_event ev;
    memset(&ev,0,sizeof(ev));
    ev.events=EPOLLIN;
    ev.data.fd=fd;
    int ret =epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&ev);
    ERROR_CHECK(ret,-1,"epoll_ctl");
    return 0;
}
