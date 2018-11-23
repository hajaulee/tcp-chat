#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include "string-constant.h"
#include "gui-design.c"
#include "client.c"

GtkWidget *window;
GtkWidget *frame;
GtkWidget *chatArea;
GtkWidget *messageInput;

char *you = "abc";
char *onlineUsers[] = {"hajau", "tuanx", "agihi", "sep", "baclaocong", "hotboyxx", "occho"};
int onlineUserCount = 7;

int main(int argc, char const *argv[])
{
	createClient();
	gtk_init(&argc, &argv);
	showLoginDialog();
	gtk_main();

	return 0;
}