#include <func.h>

#define MAXLINE 256

int main(int argc, char* argv[])
{
    int fd1 = open("pipe1", O_RDONLY);
    if (fd1 == -1) {
        error(1, errno, "open pipe1");
    }

    int fd2 = open("pipe2", O_WRONLY);
    if (fd2 == -1) {
        error(1, errno, "open pipe2");
    }

    printf("Established\n");

    char recvline[MAXLINE];
    char sendline[MAXLINE];

    fd_set mainfds; // 局部变量
    FD_ZERO(&mainfds);   // 将所有的位置为0
    FD_SET(STDIN_FILENO, &mainfds);
    int maxfds = STDIN_FILENO;

    FD_SET(fd1, &mainfds);
    if (fd1 > maxfds) {
        maxfds = fd1;
    }


    for (;;) {
        fd_set readfds = mainfds;   // 结构体的复制

        // struct timeval timeout = {5, 0};
        // int events = select(maxfds + 1, &readfds, NULL, NULL, &timeout);
        int events = select(maxfds + 1, &readfds, NULL, NULL, NULL);
        switch (events) {
        case -1:
            error(1, errno, "select");
        case 0:
            // 超时
            printf("TIMEOUT\n");
            continue;
        default:
            /* printf("timeout: tv_sec = %ld, tv_usec = %ld\n", */
            /*        timeout.tv_sec, timeout.tv_usec); */

            // STDIN_FILENO 就绪
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                // 一定不会阻塞
                fgets(sendline, MAXLINE, stdin);
                write(fd2, sendline, strlen(sendline) + 1); // +1 for '\0'
            }
            // pipe1就绪
            if (FD_ISSET(fd1, &readfds)) {
                // 一定不会阻塞
                int nbytes = read(fd1, recvline, MAXLINE);
                if (nbytes == 0) {
                    // 管道的写端关闭的
                    goto end;
                } else if (nbytes == -1) {
                    error(1, errno, "read pipe1");
                }
                printf("from p1: %s", recvline);
            }
        }
    }

end:
    close(fd1);
    close(fd2);
    
    return 0;
}


