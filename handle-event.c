#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "string-constant.h"
#include "integer-constant.h"

#define runInUIThread(x) gdk_threads_enter(); x; gdk_threads_leave();

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
extern void clearBuf(char *);
extern int sendRequest();
extern void showLoginDialog();
extern void showMessage(GtkWidget *, GtkMessageType, char *, char *);
extern void showMainWindow();
extern void updateUserList(char[][32], int);
extern int setButtonFocus(GtkWidget *, char *);
char username[100];
char password[100];

void onLogoutButtonClicked(GtkWidget *widget, gpointer *data)
{
	// destroySomething(NULL, window);
	gtk_widget_hide(window);
	clearBuf(inBuf);
	sprintf(inBuf, "%c", LOGOUT_ACTION);
	sendRequest();
	showLoginDialog();
}

void onLoginSuccess(char *message)
{
	//success
	you = username;
	runInUIThread( showMessage(loginDialog, GTK_MESSAGE_WARNING, LOGIN_SUCCESS, WELLCOME));
	runInUIThread(gtk_entry_set_text(GTK_ENTRY(inputPassword), BLANK));
	runInUIThread(showMainWindow());
	gtk_widget_set_visible(loginDialog, FALSE);
	gtk_label_set_text(GTK_LABEL(yournameLabel), username);
	clearBuf(inBuf);
	sprintf(inBuf, "%c", GET_PUBLIC_STREAM);
	sendRequest();

	// g_mutex_unlock(&mutex_interface);
}

void onForceLogout(char *message)
{
	gtk_widget_hide(window);
	showLoginDialog();
	showMessage(loginDialog, GTK_MESSAGE_WARNING, FORCE_LOGOUT, message);
}
void onSentUsername()
{
	clearBuf(inBuf);
	sprintf(inBuf, "%c%s", SEND_PASSWORD_ACTION, password);
	sendRequest();
}
void onLoginFailed(char *message)
{
	//invalid
	showMessage(loginDialog, GTK_MESSAGE_ERROR, LOGIN_FAILED, message);
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
	currentChannel = (char *)data;
	updateUserList(onlineUsers, onlineUserCount);
	if (strcmp(currentChannel, PUBLIC) == 0)
		setButtonFocus(publicChannelButton, DOWN);
	showMessage(window, GTK_MESSAGE_INFO, "haha", currentChannel);
}

void onExit(GtkWidget *widget, gpointer data)
{
	exit(0);
}
void onSendButtonClicked(GtkWidget *widget, gpointer data)
{
	clearBuf(inBuf);
	char text[100];
	strcpy(text, (char *)gtk_entry_get_text(GTK_ENTRY(messageInput)));
	gtk_entry_set_text(GTK_ENTRY(messageInput), BLANK);
	if (strcmp(currentChannel, PUBLIC) == 0)
		sprintf(inBuf, "%c%s", CHANNEL_MESSAGE_ACTION, text);
	else
		sprintf(inBuf, "%c%s#%s", PRIVATE_MESSAGE_ACTION, currentChannel, text);
	sendRequest();
}