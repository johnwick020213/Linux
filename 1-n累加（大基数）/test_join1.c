#include <func.h>

typedef struct
{
    int left;
    int right;
}Section;

void *start_routine(void* args)
{
    Section*sec =(Section*) args;
    int sum=0;
    for(int i=sec->left;i<=sec->right;i++)
    {
        sum+=i;
    }
    return (void*)sum;
}

int main(int argc,char*argv[])
{
    long long n;
    printf("Please input a long long int:");
    scanf("%lld",&n);
    
    pthread_t tid1,tid2;
    Section sec1={0,n/2};
    Section sec2={(n/2)+1,n};

    int err=pthread_create(&tid1,NULL,start_routine,&sec1);
    if(err)
    {
        error(1,err,"pthread_create");
    }

    err=pthread_create(&tid2,NULL,start_routine,&sec2);
    if(err)
    {
        error(1,err,"pthread_create");
    }

    int result1;
    err=pthread_join(tid1,(void**)&result1);
    if(err)
    {
        error(1,err,"pthread_join %lu\n",tid1);
    }

    int result2;
    err=pthread_join(tid2,(void**)&result2);
    if(err)
    {
        error(1,err,"pthread_join %lu\n",tid2);

    }

    printf("main:sum=%d\n",result1+result2);

    return 0;
}

