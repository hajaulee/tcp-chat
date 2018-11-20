#include <gtk/gtk.h>
#include "string-constant.h"
#include "integer-constant.h"

extern GtkWidget *window;
extern GtkWidget *frame;
extern GtkWidget *userListBox;
extern char inBuf[MAXLINE];
extern GtkWidget *initUserList(int, int, char *[], int);
extern void addButtonToUserListBox(char *[], int);
GtkWidget *loginDialog;
GtkWidget *inputUsername;
GtkWidget *inputPassword;
void onLogoutButtonClicked(GtkWidget *widget, gpointer *data)
{
	destroySomething(NULL, window);
	showLoginDialog();
}

/**
Handle user click to login button
@param 	widget: button clicked
		gp: passed data array
*/

void onLoginSuccess(char *message)
{
	//success
	showMessage(loginDialog, GTK_MESSAGE_WARNING, LOGIN_SUCCESS, message);
	destroySomething(NULL, loginDialog);
	showMainWindow();
}
void onSentUsername()
{
	char *password = gtk_entry_get_text(inputPassword);
	clearBuf(inBuf);
	sprintf(inBuf, "%c%s", SEND_PASSWORD_ACTION, password);
	sendRequest();
}
void onLoginFailed(char *message)
{
	//invalid
	if (loginDialog == NULL)
		printf("I am idiot");
	// showMessage(loginDialog, GTK_MESSAGE_ERROR, LOGIN_FAILED, message);
	GtkWidget * x = gtk_message_dialog_new (NULL,
                        GTK_DIALOG_MODAL|GTK_DIALOG_USE_HEADER_BAR,
                        GTK_MESSAGE_WARNING,
                        GTK_BUTTONS_OK_CANCEL,
                        "const gchar *message_format");
	gtk_widget_show(x);
	printf(">>>>>>>>heelo<<<<<<<");
}
void onLoginButtonClicked(GtkWidget *widget, gpointer gp)
{
	loginDialog = ((GtkWidget **)gp)[2];
	inputUsername = ((GtkWidget **)gp)[0];
	inputPassword = ((GtkWidget **)gp)[1];

	char *username = gtk_entry_get_text(inputUsername);
	char *password = gtk_entry_get_text(inputPassword);
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