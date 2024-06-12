#include<asm-generic/socket.h>
#include<func.h>

int main()
{
    //创建监听套接字
    int listenfd=socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(listenfd,-1,"socket");

    struct sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(8080);
    serveraddr.sin_addr.s_addr=inet_addr("192.168.***。***");

    //设置网络重用
    int on=1;
    int ret=setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
    ERROR_CHECK(ret,-1,"setsockopt");

    //绑定网路地址
    ret=bind(listenfd,(const struct sockaddr*)&serveraddr,sizeof(serveraddr));
    ERROR_CHECK(ret,-1,"bind");

    //进行监听
    ret=listen(listenfd,10);
    ERROR_CHECK(ret,-1,"listen");

    while(1)
    {
        struct sockaddr_in clientAddr;
        memset(&clientAddr,0,sizeof(clientAddr));
        socklen_t len=sizeof(clientAddr);
        int peerfd=accept(listenfd,(struct sockaddr*)&clientAddr,&len);
        printf("%s:%d已经连接。\n",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));

        //1.获取http请求报文
        char buff[4096]={0};
        int ret=recv(peerfd,buff,sizeof(buff),0);
        printf("ret:%d bytes.\n%s\n",ret,buff);

        //2.请求报文进行响应操作
        const char*startline="HTTP/1.1 200 OK\r\n";
        const char*headers="Server:MyHttpServer1.0\r\n"
            "Content-Type:text/html\r\n"
            "Content-Length:";
        const char*emptyLine="\r\n";
        const char *body=
            "<html><head>This is a test</head>"
            "<body>hello,client</body><html>";
        //清除缓冲区
        memset(buff,0,sizeof(buff));
        sprintf(buff,"%s%s%ld\r\n%s%s",startline,headers,strlen(body),emptyLine,body);
        printf("response:\r\n%s\n",buff);
        ret=send(peerfd,buff,strlen(buff),0);
        printf("\nsend%d bytes.\n",ret);
    }
    close(listenfd);
}
