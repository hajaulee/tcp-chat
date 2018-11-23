#include <gtk/gtk.h>
#include "string-constant.h"
#include "integer-constant.h"

extern GtkWidget *window;
extern GtkWidget *frame;
extern GtkWidget *userListBox;
extern char *inBuf;
extern GtkWidget *initUserList(int, int, char *[], int);
extern void addButtonToUserListBox(char *[], int);
extern void sendThread(char*);
extern GtkWidget *loginDialog;
extern GtkWidget *inputUsername;
extern GtkWidget *inputPassword;
extern 	GtkWidget *yournameLabel;
char *username;
char *password;


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
	showMessage(loginDialog, GTK_MESSAGE_WARNING, LOGIN_SUCCESS, message);
	gtk_entry_set_text(inputPassword, BLANK);
	gtk_widget_hide(loginDialog);
	showMainWindow();
	gtk_label_set_text(yournameLabel, username);

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
	username = gtk_entry_get_text(inputUsername);
	password = gtk_entry_get_text(inputPassword);

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
	char *channel = (char *)data;
	showMessage(window, GTK_MESSAGE_INFO, "haha", channel);
}

void onSendButtonClicked(GtkWidget *widget, gpointer data)
{
	char *newlist[] = {"admin", "user"};
	updateUserList(newlist, 2);
}