#include <stdio.h>
#include<errno.h>
#include<error.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<sys/mman.h>

#define MMAP_SIZE (4096*10)

int main(int argc,char*argv[])
{
    
    if(argc!=3)
    {
        error(1,0,"Usage:%s src dst",argv[0]);
    }

    int src_fd=open(argv[1],O_RDONLY);
    if(src_fd==-1)
    {
        error(1,errno,"open %s",src_fd);
    }

    int dst_fd=open(argv[2],O_RDWR|O_CREAT|_IOS_TRUNC,0666);
    if(dst_fd==-1)
    {
        close(src_fd);
        error(1,errno,"open %s",argv[2]);
    }

    struct stat sb;
    fstat(src_fd,&sb);
    off_t fsize =sb.st_size;
    ftruncate(dst_fd,fsize);
    
    off_t offset;
    while(offset<fsize)
    {
        off_t length;
        if(fsize-offset>=MMAP_SIZE)
        {
            length=MMAP_SIZE;
        }
        else
        {
            length =fsize-offset;
        }

        void*addr1=mmap(NULL,length,PROT_READ,MAP_SHARED,src_fd,offset);
        if(addr1==MAP_FAILED)
        {
            error(1,errno,"mmap %s",argv[1]);
        }

        void*addr2=mmap(NULL,length,PROT_READ|PROT_WRITE,MAP_SHARED,dst_fd,offset);
        if(addr2==MAP_FAILED)
        {
            error(1,errno,"mmap %s",argv[2]);
        }

        memcpy(addr2,addr1,length);
        offset+=length;

        int err=munmap(addr1,length);
        if(err)
        {
            error(1,errno,"munmap %s",argv[1]);
        }

        err=munmap(addr2,length);
        if(err)
        {
            error(1,errno,"munmap %s",argv[2]);
        }
    }
    return 0;
}

