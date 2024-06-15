#include"process_pool.h"

#define FILENAME "small_file.txt"

int transferFile(int peerfd)
{
    //读取本地文件
    int fd=open(FILENAME,O_RDWR);
    ERROR_CHECK(fd,-1,"open");

    train_t t;
    memset(&t,0,sizeof(t));
    //先发送文件名
    t.len=strlen(FILENAME);
    strcpy(t.buff,FILENAME);
    send(peerfd,&t,4+t.len,0);

    //再发送文件内容
    char buff[20]={0};
    int ret=read(fd,buff,sizeof(buff));
    t.len=ret;
    strncpy(t.buff,buff,ret);
    send(peerfd,&t,4+t.len,0);

    close(fd);
    return 0;
}
