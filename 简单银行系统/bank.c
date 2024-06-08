#include <func.h>

typedef struct{
    int id;
    int money;
    pthread_mutex_t mutex;
}Account;

Account acct1={1,100,PTHREAD_MUTEX_INITIALIZER};

int withdraw(Account* acct,int money)
{
    pthread_mutex_lock(&acct->mutex);
    if(acct->money<money)
    {
        pthread_mutex_unlock(&acct->mutex);
        return 0;
    }

    sleep(1);

    acct->money-=money;
    pthread_mutex_unlock(&acct->mutex);

    printf("%lu:withdraw %d\n",pthread_self(),money);
    return money;
}

void* start_routine(void* args)
{
    withdraw(&acct1,100);
    return NULL;
}
int main(int argc,char*argv[])
{
    pthread_t tid1,tid2,tid3,tid4,tid5,tid6,tid7,tid8,tid9,tid10;
    pthread_create(&tid1,NULL,start_routine,NULL);
    pthread_create(&tid2,NULL,start_routine,NULL);
    pthread_create(&tid3,NULL,start_routine,NULL);
    pthread_create(&tid4,NULL,start_routine,NULL);
    pthread_create(&tid5,NULL,start_routine,NULL);
    pthread_create(&tid6,NULL,start_routine,NULL);
    pthread_create(&tid7,NULL,start_routine,NULL);
    pthread_create(&tid8,NULL,start_routine,NULL);
    pthread_create(&tid9,NULL,start_routine,NULL);
    pthread_create(&tid10,NULL,start_routine,NULL);

    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);
    pthread_join(tid4,NULL);
    pthread_join(tid5,NULL);
    pthread_join(tid6,NULL);
    pthread_join(tid7,NULL);
    pthread_join(tid8,NULL);
    pthread_join(tid9,NULL);
    pthread_join(tid10,NULL);

    printf("balance:%d\n",acct1.money);
    return 0;
}

