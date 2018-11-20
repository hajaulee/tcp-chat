#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "integer-constant.h"
#include "string-constant.h"

int client_sock_fd;
char inBuf[MAXLINE];
char outBuf[MAXLINE];

void clearBuf(char buff[]){
    memset(buff, 0, strlen(buff));
}
int sendRequest()
{
    printf("Prepare:%s\n", inBuf);
    return send(client_sock_fd, inBuf, strlen(inBuf), 0);
}
void signio_handler(int signo)
{
    char action, *message, *value, buff[MAXLINE];
    char *split;
    int n = recv(client_sock_fd, buff, sizeof buff, 0);
    if (n > 0)
    {
        buff[n] = 0;
        if (buff[strlen(buff) - 1] == '\n')
            buff[strlen(buff) - 1] = '\0';
        action = buff[0];
        message = buff + 1;
        puts(buff);
        switch (action)
        {
        case LOGIN_RESPONSE_ACTION:
            split = strchr(message, '#');
            *split = '\0';
            value = message;
            message = split + 1;
            printf("value:%s\nmessage:%s\n", value, message);
            if (strcmp(value, SUCCESS) == 0)
            {
                if (strcmp(message, OK) == 0)// username Sent
                {
                    onSentUsername();
                }
                else //password sent
                {
                    onLoginSuccess(message);
                }
            }
            else
            {
                onLoginFailed(message);
            }
            break;
        }
    }
    else
    {
        puts("Error!!!!!");
    }
}
int createClient()
{
    struct sockaddr_in client_socket;
    struct sockaddr_in server_socket;
    client_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_socket.sin_family = AF_INET;
    server_socket.sin_port = htons(SERV_PORT);
    server_socket.sin_addr.s_addr = inet_addr("127.0.0.1");
    printf("server IP = %s ", inet_ntoa(server_socket.sin_addr));

    if (connect(client_sock_fd, (struct sockaddr *)&server_socket, sizeof(server_socket)) < 0)
        printf("Error in connecting to server\n");
    else
        printf("connected to server\n");

    // Signal driven I/O mode and NONBlOCK mode so that recv will not block
    if (fcntl(client_sock_fd, F_SETFL, O_NONBLOCK | O_ASYNC))
        printf("Error in setting socket to async, nonblock mode");

    signal(SIGIO, signio_handler); // assign SIGIO to the handler

    //set this process to be the process owner for SIGIO signal
    if (fcntl(client_sock_fd, F_SETOWN, getpid()) < 0)
        printf("Error in setting own to socket");
}
