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
// void * timer_exe(gpointer);
int main(int argc, char *argv[])
{

	//to be thread-aware
    if (!g_thread_supported ()){ g_thread_init(NULL); }
	// initialize GDK thread support
	gdk_threads_init();

	// acquire the GDK lock
	gdk_threads_enter();

	publicStream = (char *)malloc(1024 * MAXLINE);
	createClient();
	g_timeout_add(100, (GSourceFunc)timer_exe, NULL);
	gtk_init(&argc, &argv);
	// gdk_threads_enter();
	initMainWindow();
	showLoginDialog();


	gtk_main();

	// release the GDK lock
	gdk_threads_leave();
	return 0;
}