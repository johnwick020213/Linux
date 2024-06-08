#include<func.h>

int main()
{
    int clientfd=socket(AF_INET,SOCK_DGRAM,0);
    ERROR_CHECK(clientfd,-1,"socket");

    //跨标签复制数据时，需要shift+insert
    struct sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(8080);
    serveraddr.sin_addr.s_addr=inet_addr("192.168.127.129");
    socklen_t len=sizeof(serveraddr);

    char buff[128]="你好，服务器";
    int ret=sendto(clientfd,buff,strlen(buff),0,(const struct sockaddr*)&serveraddr,sizeof(serveraddr));

    //第一次recvfrom时，只能接收13byte的数据
    //但用户态的缓冲区空间是足够的，有128个字节。
    memset(buff,0,sizeof(buff));
    ret=recvfrom(clientfd,buff,sizeof(buff),0,(struct sockaddr*)&serveraddr,&len);
    printf("接收到%d个字节的消息。\n",ret);
    printf("消息：%s\n",buff);
    while(1);
    close(clientfd);
    return 0;
}
