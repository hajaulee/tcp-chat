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
#include <sys/select.h>
#include <sys/time.h>
#include <netdb.h>
#include "integer-constant.h"
#include "string-constant.h"

int client_sock_fd;
char *inBuf;
char outBuf[4096];
extern char onlineUsers[USER_NUM_MAX][32];
extern int onlineUserCount;
extern void updateUserList(char n[][32], int);

void clearBuf(char *buff)
{
    memset(buff, 0, MAXLINE);
}
int sendRequest()
{
    printf("Send to server :{%s}\n", inBuf);
    return send(client_sock_fd, inBuf, strlen(inBuf), 0);
}
int handleLoginResponse(char *message)
{
    char *split = strchr(message, '#');
    *split = '\0';
    char *value = message;
    message = split + 1;
    printf("value:%s\nmessage:%s\n", value, message);
    if (strcmp(value, SUCCESS) == 0)
    {
        if (strcmp(message, OK) == 0) // username Sent
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
}

int handleOnlineUsersList(char *message)
{
    int i, j = 0;
    int messageLength = strlen(message);
    onlineUserCount = 0;
    for (i = 0; i < messageLength; i++)
    {
        printf("i=%d\n", i);
        if (message[i] == SEPARATOR)
        {
            message[i] = '\0';
            strcpy(onlineUsers[onlineUserCount], message + j);
            printf(">>>%s<<<\n", onlineUsers[onlineUserCount]);
            j = i + 1;
            onlineUserCount++;
            printf("\n>>%d\n", onlineUserCount);
        }
        if (message[i + 1] == '\0')
            break;
        printf("%c", message[i]);
    }
    printf("Count:%d", onlineUserCount);
    updateUserList(onlineUsers, onlineUserCount);
}
void handleReponse(char *buff, int n)
{
    printf("Received form server:\"%s\"\n", buff);
    char action, *message, *value;
    buff[n] = 0;
    if (buff[strlen(buff) - 1] == '\n')
        buff[strlen(buff) - 1] = '\0';
    action = buff[0];
    message = buff + 1;
    puts(buff);
    switch (action)
    {
    case LOGIN_RESPONSE_ACTION:
        handleLoginResponse(message);
        break;
    case GET_LIST_USER_ACTION:
        handleOnlineUsersList(message);
        break;
    }
}
void signio_handler(int signo)
{
    char buffer[4096];
    clearBuf(buffer);
    int n = recv(client_sock_fd, buffer, MAXLINE, 0);
    if (n > 0)
    {
        handleReponse(buffer, n);
    }
    else
    {
        puts("Error!!!!!");
    }
}
int createClient()
{

    inBuf = (char *)malloc(MAXLINE * sizeof(char));
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

    // set this process to be the process owner for SIGIO signal
    if (fcntl(client_sock_fd, F_SETOWN, getpid()) < 0)
        printf("Error in setting own to socket");
}