#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "string-constant.h"
#include "integer-constant.h"
#include "client.c"

#define runInUIThread(x) \
	gdk_threads_enter(); \
	x;                   \
	gdk_threads_leave();

extern GtkWidget *window;
extern GtkWidget *frame;
extern GtkWidget *userListBox;
extern char *inBuf;
extern GtkWidget *initUserList(int, int, char[][32], int);
extern void addButtonToUserListBox(char[][32], int);
extern void sendThread(char *);
extern GtkWidget *loginDialog;
extern GtkWidget *inputUsername;
extern GtkWidget *inputPassword;
extern GtkWidget *yournameLabel;
extern GtkWidget *messageInput;
extern char *you;
extern char onlineUsers[USER_NUM_MAX][32];
extern int onlineUserCount;
extern GtkWidget *publicChannelButton;
extern char *currentChannel;
extern char *publicStream;
extern GtkWidget *chatArea;
extern void clearBuf(char *);
extern int sendRequest();
extern void showLoginDialog();
extern void showMessage(GtkWidget *, GtkMessageType, char *, char *);
extern char * saveToUserMessageStream(char *, char *);
extern void showMainWindow();
extern void updateUserList(char[][32], int);
extern int setButtonFocus(GtkWidget *, char *);
extern int findUserMessageStream(char * );
extern User onlineUsersStream[USER_NUM_MAX];
char username[100];
char password[100];

void clearStreamList(){
	int i; 
	for(i = 0; i < USER_NUM_MAX; i++){
		if(strlen(onlineUsersStream[i].username) > 0){
			memset(onlineUsersStream[i].username, 0, strlen(onlineUsersStream[i].username));
			free(onlineUsersStream[i].stream);
		}
	}
}
void onLogoutButtonClicked(GtkWidget *widget, gpointer *data)
{
	// destroySomething(NULL, window);
	gtk_widget_hide(window);
	clearBuf(inBuf);
	clearStreamList();
	currentChannel = PUBLIC;
	sprintf(inBuf, "%c", LOGOUT_ACTION);
	sendRequest();
	showLoginDialog();
}

void onLoginSuccess(char *message)
{
	//success
	runInUIThread(showMessage(loginDialog, GTK_MESSAGE_WARNING, LOGIN_SUCCESS, WELLCOME));
	runInUIThread(gtk_entry_set_text(GTK_ENTRY(inputPassword), BLANK));
	runInUIThread(showMainWindow());
	runInUIThread(gtk_widget_set_visible(loginDialog, FALSE);
				  gtk_label_set_text(GTK_LABEL(yournameLabel), username););
	clearBuf(inBuf);
	sprintf(inBuf, "%c", GET_PUBLIC_STREAM);
	sendRequest();
}

void *showBubbleNotify(void *notify){	
	char command[200];
	sprintf(command, "notify-send \"%s\"", notify);
	system(command);
}
void onUserTagged(char * sender){
	char notify[69];
	sprintf(notify, TAGGED_NOTIFY, sender);
	showBubbleNotify(notify);
}

void onSentUsername()
{
	you = username;
	clearBuf(inBuf);
	sprintf(inBuf, "%c%s", SEND_PASSWORD_ACTION, password);
	sendRequest();
}
void onLoginFailed(char *message)
{
	//invalid
	runInUIThread(showMessage(loginDialog, GTK_MESSAGE_ERROR, LOGIN_FAILED, message));
	// showMessage(loginDialog, GTK_MESSAGE_ERROR, LOGIN_FAILED, message);
}
void onLoginButtonClicked(GtkWidget *widget, gpointer gp)
{
	strcpy(username, (char *)gtk_entry_get_text(GTK_ENTRY(inputUsername)));
	strcpy(password, (char *)gtk_entry_get_text(GTK_ENTRY(inputPassword)));

	if (strlen(username) < 1 || strlen(password) < 1)
		showMessage(loginDialog, GTK_MESSAGE_WARNING, LOGIN_FAILED, NOT_EMPTY);
	else
	{
		clearBuf(inBuf);
		sprintf(inBuf, "%c%s", SEND_USER_ACTION, username);
		sendRequest();
	}
}

void onChannelButtonClicked(GtkWidget *widget, gpointer data)
{
	int i = 0, count ;
	char name[50];
	currentChannel = (char *)data;
	char * x = strchr(currentChannel, BRACKET);
	if (x != NULL)
	{
		*x = '\0'; // Remove message count from user name example: hau(2) => hau
	}
	for(i = 0; i< onlineUserCount; i++){
		puts(onlineUsers[i]);
        sscanf(onlineUsers[i], "%[^(](%d", name, &count);
        if (strcmp(name, currentChannel) == 0)
        {
        	puts(onlineUsers[i]);
            strcpy(onlineUsers[i], currentChannel);
            break;
        }
    }
	if (strcmp(currentChannel, PUBLIC) == 0)
	{
		printf("setButtonFocus\n");
		setButtonFocus(publicChannelButton, DOWN);
		textViewSetText(chatArea, publicStream);
	}else{
		int id = findUserMessageStream(currentChannel);
		if (id != -1)
		{
			onlineUsersStream[id].newMessage = 0;
			textViewSetText(chatArea, onlineUsersStream[id].stream);
		}
	}
	updateUserList(onlineUsers, onlineUserCount);
	// showMessage(window, GTK_MESSAGE_INFO, "haha", currentChannel);
}

void onExit(GtkWidget *widget, gpointer data)
{
	exit(0);
}
void onSendButtonClicked(GtkWidget *widget, gpointer data)
{
	clearBuf(inBuf);
	char text[100];
	char *entryText;
	entryText = (char *)gtk_entry_get_text(GTK_ENTRY(messageInput));
	strcpy(text, entryText);
	gtk_entry_set_text(GTK_ENTRY(messageInput), BLANK);
	if (strcmp(currentChannel, PUBLIC) == 0)
		sprintf(inBuf, "%c%s", CHANNEL_MESSAGE_ACTION, text);
	else{
		char temp [MAXLINE];
		sprintf(temp, "%s:%s", you, text);
		char * xstream = saveToUserMessageStream(currentChannel, temp);
		printf("%s\n", xstream);
		textViewSetText(chatArea, xstream);
		sprintf(inBuf, "%c%s#%s", PRIVATE_MESSAGE_ACTION, currentChannel, text);
	}
	sendRequest();
}