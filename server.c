#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include "integer-constant.h"
#include "string-constant.h"

char buf[MAXLINE];
typedef struct $
{
    char username[32];
    char password[100];
    int fd;
} User;

User users[USER_NUM_MAX];
int auth[USER_NUM_MAX];

int sendResponse(int connfd)
{
    return send(connfd, buf, strlen(buf), 0);
}
int cleanBuffer()
{
    memset(buf, 0, MAXLINE);
}
int checkUser(char *name)
{
    int i;
    for (i = 0; i < USER_NUM_MAX; i++)
    {
        if (strcmp(name, users[i].username) == 0)
        {
            return i;
        }
    }
    return -1;
}

int markUserNameInputed(int fdnum, int userId)
{
    auth[fdnum] = -1 - userId;
}

int markUserLogged(int fdnum, int userId)
{
    auth[fdnum] = userId;
}

int validatePassword(int fdnum, char *password)
{
    if (auth[fdnum] < 0)
    {
        User u = users[-1 - auth[fdnum]];
        printf("User:{%s}{%s}{%s}{%d}\n", u.username, u.password, password, u.fd);
        if (strcmp(u.password, password) == 0)
        {
            if (u.fd != 0)
                return CODE_LOGGED_BY_ANOTHER;
            printf("%d", -1 - auth[fdnum]);
            return -1 - auth[fdnum];
        }
    }
    return CODE_PASSWORD_INCORRECT;
}
int initAuth()
{
    int i;
    for (i = 0; i < USER_NUM_MAX; i++)
    {
        auth[i] = -1;
    }
}
int initUserList()
{
    memset(users, 0, sizeof users);
    printf("Reset user list on %ld bytes\n", sizeof users);
}
int loadUserList(const char *source)
{
    FILE *f = fopen(source, "r+");
    char temp[200];
    int i;
    if (f == NULL)
    {
        puts("Read user list error");
        exit(-1);
    }
    for (i = 0; fgets(temp, 200, f) != NULL; i++)
    {
        sscanf(temp, "%s %s", users[i].username, users[i].password);
        printf("User:{\n  username:\"%s\"\n  password:\"%s\"\n}\n", users[i].username, users[i].password);
    }
}

void logoutUser(int connfd)
{
    int userId = auth[connfd];
    users[userId].fd = 0;
    auth[connfd] = -1;
}
int handleMessage(int connfd)
{
    int n, userId;
    char action, *message;
    char readBuf[MAXLINE];
    if ((n = recv(connfd, readBuf, MAXLINE, 0)) > 0)
    {
        readBuf[n] = '\0';
        if (readBuf[strlen(readBuf) - 1] == '\n')
            readBuf[strlen(readBuf) - 1] = 0;
        printf("String received from client: %s\n", readBuf);
        action = readBuf[0];
        message = readBuf + 1;
        cleanBuffer();
        switch (action)
        {
        case SEND_USER_ACTION:
            puts("114");
            userId = checkUser(message);
            if (userId == -1)
            {
                sprintf(buf, "%c%s#%s", LOGIN_RESPONSE_ACTION, FAILED, USER_INVALID);
            }
            else
            {
                markUserNameInputed(connfd, userId);
                sprintf(buf, "%c%s#%s", LOGIN_RESPONSE_ACTION, SUCCESS, OK);
            }
            sendResponse(connfd);
            puts("126");
            break;
        case SEND_PASSWORD_ACTION:
            userId = validatePassword(connfd, message);
            if (userId == CODE_PASSWORD_INCORRECT) // password incorrect
            {
                sprintf(buf, "%c%s#%s", LOGIN_RESPONSE_ACTION, FAILED, ACCOUNT_INVALID);
            }
            else
            {
                if (userId == CODE_LOGGED_BY_ANOTHER) // Account are Already logged by other client
                {
                    sprintf(buf, "%c%s#%s", LOGIN_RESPONSE_ACTION, FAILED, SESSION_INVALID);
                    User u = users[-1 - auth[connfd]];
                    sendResponse(u.fd);
                    u.fd = connfd;
                    auth[connfd] = -1 - auth[connfd];

                    cleanBuffer();
                    sprintf(buf, "%c%s#%s", LOGIN_RESPONSE_ACTION, SUCCESS, SESSION_INVALID);
                    sendResponse(connfd);
                }
                else // Account logged normaly
                {
                    sprintf(buf, "%c%s#%s", LOGIN_RESPONSE_ACTION, SUCCESS, WELLCOME);
                    auth[connfd] = userId;
                }
            }
            sendResponse(connfd);
            break;
        case LOGOUT_ACTION:
            logoutUser(connfd);
            break;
        }
    }
    return n;
}

int handleNewConnection(int connfd)
{
    return 0;
}
int createServe()
{
    int maxfd, listenfd, connfd, n, i, rv;
    socklen_t clilen;
    fd_set readfds, master;
    struct sockaddr_in cliaddr, servaddr;

    //creation of the socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    //preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) >= 0)
    {
        puts("Server address is maybe:");
        system("ifconfig | perl -nle'/dr:(\\S+)/ && print $1'");
        printf("Server is running at port %d\n", SERV_PORT);
    }
    else
    {
        perror("bind failed");
        return 0;
    }

    listen(listenfd, USER_NUM_MAX);

    printf("%s\n", "Server running...waiting for connections.");

    fprintf(stdout, "1hello: %d\n", maxfd);
    fflush(stdout);
    //Assign initial value for the fd_set
    maxfd = listenfd;
    FD_ZERO(&master);  //FD_ZERO works like memset 0;
    FD_ZERO(&readfds); //clear the master and temp sets
    FD_SET(maxfd, &master);
    while (1)
    {

        readfds = master;
        // // clear the set ahead of time
        //      FD_ZERO(&readfds);
        rv = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (rv == -1)
        {
            puts("error");
        }
        for (i = 0; i <= maxfd; i++)
        {
            if (FD_ISSET(i, &readfds))
            {
                if (i == listenfd)
                {
                    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
                    if (connfd == -1)
                    {
                        perror("accept");
                    }
                    else
                    {
                        if (connfd > maxfd)
                        { // keep track of the max
                            maxfd = connfd;
                        }
                        FD_SET(connfd, &master);
                        printf("New connection on socket %d\n", connfd);
                        handleNewConnection(connfd);
                    }
                }
                else
                {
                    int r = handleMessage(i);
                    if (r <= 0)
                    { // got error or connection closed by client
                        if (r == 0)
                        { // connection closed
                            printf("Socket %d hung up\n", i);
                        }
                        else
                        {
                            perror("recv");
                        }
                        close(i);           // bye!
                        FD_CLR(i, &master); // remove from master set
                        auth[i] = -1;
                    }
                }
            }
        }
    }
}

int main(int argc, char **argv)
{
    loadUserList("users.txt");
    initAuth();
    createServe();
}