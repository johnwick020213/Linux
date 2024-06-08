#include<func.h>
#include<sys/select.h>
#include<unistd.h>

int main()
{
    //创建客户端套接字
    int clientfd=socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(clientfd,-1,"socket");
    printf("clientfd:%d\n",clientfd);

    struct sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(8080);
    serveraddr.sin_addr.s_addr=inet_addr("192.168.127.129");

    int ret=connect(clientfd,(const struct sockaddr*)&serveraddr,sizeof(serveraddr));
    ERROR_CHECK(ret,-1,"connect");

    printf("连接成功\n");

    fd_set rdset;
    FD_ZERO(&rdset);
    char buff[1000]={0};

    while(1)
    {
        FD_ZERO(&rdset);
        FD_SET(clientfd,&rdset);
        FD_SET(STDIN_FILENO,&rdset);

        select(clientfd+1,&rdset,NULL,NULL,NULL);

        if(FD_ISSET(STDIN_FILENO,&rdset))
        {
            memset(buff,0,sizeof(buff));
            int ret = read(STDIN_FILENO,buff,sizeof(buff));
            if(ret==0)
            {
                break;
            }
            printf("读取ret:%d\n",ret);
            ret=send(clientfd,buff,strlen(buff)-1,0);
            printf("发送了%d个字节。\n",ret);
        }

        if(FD_ISSET(clientfd,&rdset))
        {
            memset(buff,0,sizeof(buff));
            int ret=recv(clientfd,buff,sizeof(buff),0);
            if(ret==0)
            {
                break;
            }
            printf("接收信息：%s\n",buff);

        }
    }
    printf("再见。\n");

    close(clientfd);
    return 0;
}
