#include <func.h>

int main()
{
    //创建客户端套接字
    int clientfd=socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(clientfd,-1,"socket");
    
    struct sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(8080);
    serveraddr.sin_addr.s_addr=inet_addr("192.168.127.129");

    int ret =connect(clientfd,(const struct sockaddr*)&serveraddr,sizeof(serveraddr));
    ERROR_CHECK(ret,-1,"connect");

    //先接收文件名
    char filename[100]={0};
    int len=0;
    ret=recv(clientfd,&len,sizeof(len),0);
    printf("ret:%d,filename's len:%d\n",ret,len);
    ret=recv(clientfd,filename,len,0);
    printf("ret:%d,recv msg:%s\n",ret,filename);
    //再接收文件内容
    char buff[100]={0};
    ret=recv(clientfd,&len,sizeof(len),0);
    printf("ret:%d,file content's len:%d\n",ret,len);
    ret=recv(clientfd,buff,len,0);
    printf("ret:%d,recv msg:%s\n",ret,buff);
    //最后写入本地
    int wfd=open(filename,O_CREAT|O_RDWR,0644);
    ERROR_CHECK(wfd,-1,"open");
    write(wfd,buff,ret);

    close(clientfd);

    return 0;
}
