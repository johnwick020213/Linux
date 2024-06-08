#include <func.h>
#define MAX_CMD_LEN 1024

int main(int argc,char*argv[])
{
    char cmd[MAX_CMD_LEN];

    for(;;)
    {
        printf(">");

        if(fgets(cmd,sizeof(cmd),stdin)==NULL)
            break;
    }

    cmd[strcspn(cmd,"\n")]='\0';

    if(strcmp(cmd,"exit")==0)
    {
        break;
    }

    pid_t pid=fork();
    if(pid<0)
    {
        error(1,errno,"fork failed");
    }
    else if(pid==0)
    {
        char *args[]={"/bin/sh","-c",cmd,NULL};
        execvp(args[0],args);
        error(1,errno,"execvp failed");
    }
    else
    {
        int status;
        if(waitpid(pid,&status,0)==-1)
        {
            error(1,errno,"waitpid failed");
        }

        if (WIFEXITED(status))
        {
            printf("Child exited with status %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("Child killed by signal %d\n", WTERMSIG(status));
        }
        else if (WIFSTOPPED(status))
        {
            printf("Child stopped by signal %d\n", WSTOPSIG(status));
        }
        else
        {
            printf("Child terminated abnormally\n");
        }
    }
    return 0;
}

