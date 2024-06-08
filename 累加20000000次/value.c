#include <func.h>

pthread_mutex_t mutex =PTHREAD_MUTEX_INITIALIZER;

void* start_routine(void* args)
{
    long*value=(long*)args;
    for(long i=0;i<10000000;i++)
    {
        pthread_mutex_lock(&mutex);
        (*value)++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc,char*argv[])
{
    long long* value =(long long*)calloc(1,sizeof(long long));

    pthread_t tid1,tid2;
    pthread_create(&tid1,NULL,start_routine,value);
    pthread_create(&tid2,NULL,start_routine,value);

    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_mutex_destroy(&mutex);

    printf("value=%lld\n",*value);
    return 0;
}

