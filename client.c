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

int client_sock_fd, wait = 9;
char *inBuf;
char outBuf[4096];
extern char onlineUsers[USER_NUM_MAX][32];
extern int onlineUserCount;
extern char *publicStream;
extern GtkWidget *chatArea;
extern void updateUserList(char n[][32], int);
extern void textViewSetText(GtkWidget *, char *);
extern char *currentChannel;
void clearBuf(char *buff)
{
    memset(buff, 0, MAXLINE);
}
int sendRequest()
{
    printf("Send to server :{%s}\n", inBuf);
    return send(client_sock_fd, inBuf, strlen(inBuf), 0);
}
char *splitMessage(char *message)
{
    char *split = strchr(message, '#');
    *split = '\0';
    return split + 1;
}
int handleLoginResponse(char *_message)
{
    char *value = _message;
    _message = splitMessage(_message);
    printf("value:%s\nmessage:%s\n", value, _message);
    if (strcmp(value, SUCCESS) == 0)
    {
        if (strcmp(_message, OK) == 0) // username Sent
        {
            onSentUsername();
        }
        else //password sent
        {
            onLoginSuccess(_message);
        }
    }
    else
    {
        onLoginFailed(_message);
    }
    return 0;
}

int handlePrivateMessage(char *message)
{
    char *sender = message;
    message = splitMessage(message);
    return 0;
}

int handlePublicMessage(char *message)
{
    char *sender = message;
    message = splitMessage(message);
    char temp[MAXLINE];
    sprintf(temp, "%s:%s\n", sender, message);
    strcat(publicStream, temp);
    if (strcmp(currentChannel, PUBLIC) == 0){
        textViewSetText(chatArea, publicStream);
    }
    return 0;
}
int handleOnlineUsersList(char *message)
{
    int i, j = 0;
    int messageLength = strlen(message);
    onlineUserCount = 0;
    for (i = 0; i < messageLength; i++)
    {
        if (message[i] == SEPARATOR)
        {
            message[i] = '\0';
            strcpy(onlineUsers[onlineUserCount], message + j);
            j = i + 1;
            onlineUserCount++;
        }
    }
    updateUserList(onlineUsers, onlineUserCount);
    return messageLength;
}

void handleGetStream(char *message)
{
    printf("___________________%s______\n", message);
    strcpy(publicStream, message);
    strcat(publicStream, "\n");
    textViewSetText(chatArea, publicStream);
}
void handleReponse(char *buff, int n)
{

    buff[n] = '\0';
    printf("Received form server:\"{%s}{length:%d}\"\n", buff, n);
    char action, *message;
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
    case PRIVATE_MESSAGE_ACTION:
        handlePrivateMessage(message);
        break;
    case CHANNEL_MESSAGE_ACTION:
        handlePublicMessage(message);
        break;
    case GET_PUBLIC_STREAM:
        printf(">><<\n", strlen(message));
        if (strlen(message) > 1)
        {
            handleGetStream(message);
        }
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
        if((wait+=3) <= 12)
            printf("\rWaiting for response.  ");
        else if (wait <= 24)
            printf("\rWaiting for response.. ");
        else{
            wait = 0;
            printf("\rWaiting for response...");
        }
    }
}

gboolean timer_exe(gpointer p){
    signio_handler(0);
    return TRUE;
}
void signal_SIGABRT(int signal)
{
    printf("SIGABRT\n");
}
int createClient()
{

    inBuf = (char *)malloc(MAXLINE * sizeof(char));
    // struct sockaddr_in client_socket;
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
    // if (fcntl(client_sock_fd, F_SETFL, O_NONBLOCK | O_ASYNC))
    if (fcntl(client_sock_fd, F_SETFL, O_NONBLOCK))
        printf("Error in setting socket to async, nonblock mode");

    // signal(SIGIO, signio_handler); // assign SIGIO to the handler
    signal(SIGABRT, signal_SIGABRT);
    // set this process to be the process owner for SIGIO signal
    if (fcntl(client_sock_fd, F_SETOWN, getpid()) < 0)
        printf("Error in setting own to socket");
    return 0;
}