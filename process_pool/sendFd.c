#include "process_pool.h"

int sendFd(int pipefd,int fd)
{
    char buff[4]={0};
    //构建第二组
    struct iovec iov;
    iov.iov_base=buff;
    iov.iov_len=sizeof(buff);
    //构建第三组
    size_t len=CMSG_LEN(sizeof(fd));
    struct cmsghdr*cmsg=(struct cmsghdr*)calloc(1,len);
    cmsg->cmsg_len=len;
    cmsg->cmsg_level=SOL_SOCKET;
    cmsg->cmsg_type=SCM_RIGHTS;
    int *p=(int*)CMSG_DATA(cmsg);
    *p=fd;

    struct msghdr msg;
    memset(&msg,0,sizeof(msg));
    msg.msg_iov=&iov;
    msg.msg_iovlen=1;
    msg.msg_control=cmsg;
    msg.msg_controllen=len;

    //发送数据
    int ret=sendmsg(pipefd,&msg,0);
    printf("sendFd %d bytes.\n",ret);
    free(cmsg);
    return 0;
}

int recvFd(int pipefd,int*pfd)
{
    char buff[4]={0};
    //构建第二组
    struct iovec iov;
    iov.iov_base=buff;
    iov.iov_len=sizeof(buff);
    //构建第三组
    size_t len=CMSG_LEN(sizeof(int));
    struct cmsghdr*cmsg=(struct cmsghdr*)calloc(1,len);
    cmsg->cmsg_len=len;
    cmsg->cmsg_level=SOL_SOCKET;
    cmsg->cmsg_type=SCM_RIGHTS;

    struct msghdr msg;
    memset(&msg,0,sizeof(msg));
    msg.msg_iov=&iov;
    msg.msg_iovlen=1;
    msg.msg_control=cmsg;
    msg.msg_controllen=len;

    int ret=recvmsg(pipefd,&msg,0);
    printf("recvFd %d bytes.\n",ret);
    *pfd=*(int*)CMSG_DATA(cmsg);
    free(cmsg);
    return 0;
}
