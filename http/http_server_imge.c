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
    serveraddr.sin_addr.s_addr=inet_addr("192.168.127.129");

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
        int peerfd=accept(listenfd,NULL,NULL);

        //1.获取http请求报文
        char buff[4096]={0};
        ret=recv(peerfd,buff,sizeof(buff),0);
        printf("buff:\n%s\n",buff);

        //2.请求报文进行响应操作
        const char*startline="HTTP/1.1 200 OK\r\n";
        const char*headers="Server:MyHttpServer1.0\r\n"
            "Content-Type:image/jpg\r\n"
            "Content-Length:";
        const char*emptyLine="\r\n";

        int fd=open("image.jpg",O_RDONLY);
        ERROR_CHECK(fd,-1,"open");
        struct stat st;
        fstat(fd,&st);
        printf("image's length:%ld\n",st.st_size);

        memset(buff,0,sizeof(buff));
        sprintf(buff,"%s%s%ld\r\n%s",startline,headers,st.st_size,emptyLine);
        //先发送报文头信息
        ret=send(peerfd,buff,strlen(buff),0);
        printf("1 send %d bytes.\n",ret);
        //再发送报文体
        char*pbody=(char*)calloc(1,st.st_size);
        read(fd,pbody,st.st_size);

        //因为图片比较大，需要进行循环发送
        //确保所有的图片数据被客户端接收完毕
        char*pbuf=pbody;
        int sendBytes=0;
        while(sendBytes<st.st_size)
        {
            ret=send(peerfd,pbuf,st.st_size-sendBytes,0);
            sendBytes+=ret;
            pbuf+=ret;
        }
        printf("2 send %d bytes.\n",sendBytes);

        free(pbody);
    }
        close(listenfd);

        return 0;
}
