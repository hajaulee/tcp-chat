#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include "string-constant.h"
#include "gui-design.c"
#include "client.c"
GtkWidget *window = NULL;
GtkWidget *frame;
GtkWidget *chatArea;
GtkWidget *messageInput;

char *you = "abc";
char *currentChannel = PUBLIC;
char onlineUsers[USER_NUM_MAX][32];
char *messageStream[USER_NUM_MAX];
char *publicStream;
int onlineUserCount = 0;

int main(int argc, char *argv[])
{
	/* init threads */
	g_thread_init(NULL);
	gdk_threads_init();

	publicStream = (char *)malloc(1024 * MAXLINE);
	createClient();
	gtk_init(&argc, &argv);
	gdk_threads_enter();
	initMainWindow();
	showLoginDialog();

	gtk_main();
	return 0;
}