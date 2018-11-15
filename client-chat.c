#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define APP_TITLE "TCP-Chat by ASK60"
#define TEXT_INPUT_LABEL ""
#define SEND_LABEL "Gửi"
#define ONLINE_LIST_LABEL "Online"
#define YOU "You"
#define PUBLIC "Public"
#define LOGIN "Đăng nhập"
#define USERNAME "Tên đăng nhập:"
#define PASSWORD "Mật khẩu:"
#define CANCEL "Hủy"
#define LOGIN_SUCCESS "Đăng nhập thành công"
#define LOGIN_FAILED "Đăng nhập thất bại"
#define ACCOUNT_INVALID "Tài khoản hoặc mật khẩu không hợp lệ"
#define SESSION_INVALID "Tài khoản đang được đăng nhập tại máy khác sẽ bị đăng xuất"
#define WELLCOME "Chào mừng bạn đến với phần mềm chat của chúng tôi"
GtkWidget *window;
GtkWidget *frame;
GtkWidget *about_dialog;
GtkWidget *chatArea;
GtkWidget *messageInput;
GtkListStore *list;

char * you = "abc";


void set_size(GtkWidget * gw, int width, int height) {
	gtk_widget_set_size_request(gw, width, height);
}

void set_pos(GtkWidget * gw, int x, int y) {
	gtk_fixed_put(GTK_FIXED(frame), gw, x, y);
}

void destroySomething(GtkWidget * widget, gpointer gp) {
	gtk_widget_destroy(gp);
}

/**
Handle user click to login button
@param 	widget: button clicked
		gp: passed data array
*/
void onLoginButtonClicked(GtkWidget * widget, gpointer gp){
	GtkWidget *adddialog = ((GtkWidget**)gp)[2];
	GtkWidget* inputUsername = ((GtkWidget**)gp)[0];
	GtkWidget* inputPassword = ((GtkWidget**)gp)[1];

	//code here
	//success
	// showMessage(adddialog, GTK_MESSAGE_INFO, LOGIN_SUCCESS, WELLCOME );
	//invalid
	// showMessage(adddialog, GTK_MESSAGE_ERROR, LOGIN_FAILED, ACCOUNT_INVALID );
	//already login
	showMessage(adddialog, GTK_MESSAGE_WARNING, LOGIN_SUCCESS, SESSION_INVALID );

	destroySomething(NULL, adddialog);
}

void showMessage(GtkWidget * parent , GtkMessageType type,  char * mms, char * content) {
	GtkWidget *mdialog;
	mdialog = gtk_message_dialog_new(GTK_WINDOW(parent),
	                                 GTK_DIALOG_DESTROY_WITH_PARENT,
	                                 type,
	                                 GTK_BUTTONS_OK,
	                                 "%s", mms);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(mdialog), "%s",  content);
	gtk_dialog_run(GTK_DIALOG(mdialog));
	gtk_widget_destroy(mdialog);
}

void show_add_dialog() {
	GtkWidget *adddialog;
	adddialog = gtk_dialog_new_with_buttons(LOGIN, GTK_WINDOW(window),
	                                        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, NULL, NULL);

	GtkWidget *dialog_ground = gtk_fixed_new();
	GtkWidget* tframe = gtk_frame_new(USERNAME);
	GtkWidget* bframe = gtk_frame_new(PASSWORD);
	GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	GtkWidget * loginButton =  gtk_button_new_with_label(LOGIN);
	GtkWidget * cancelButton = gtk_button_new_with_label(CANCEL);
	GtkWidget* inputUsername = gtk_entry_new();
	GtkWidget* inputPassword = gtk_entry_new();
	gtk_entry_set_visibility (inputPassword, FALSE);

	gtk_box_pack_start(GTK_BOX(box), loginButton, TRUE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(box), cancelButton, TRUE, TRUE, 2);

	set_size(tframe, 300, 50);
	set_size(bframe, 300, 50);
	set_size(box, 300, 50);
	set_size(loginButton, 100, 30);
	set_size(cancelButton, 100, 30);

	gtk_widget_set_margin_left(inputUsername, 2);
	gtk_widget_set_margin_right(inputUsername, 2);
	gtk_widget_set_margin_left(inputPassword, 2);
	gtk_widget_set_margin_right(inputPassword, 2);

	gtk_fixed_put(GTK_FIXED(dialog_ground), tframe, 0, 20);
	gtk_fixed_put(GTK_FIXED(dialog_ground), bframe, 0, 80);
	gtk_fixed_put(GTK_FIXED(dialog_ground), box, 0, 220);

	gtk_container_add(GTK_CONTAINER(tframe), inputUsername);
	gtk_container_add(GTK_CONTAINER(bframe), inputPassword);

	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(adddialog))), dialog_ground,  TRUE, TRUE, 0);
	
	GtkWidget * data_array[3];
	data_array[0] = inputUsername;
	data_array[1] = inputPassword;
	data_array[2] = adddialog;
	g_signal_connect(loginButton, "clicked", G_CALLBACK(onLoginButtonClicked), data_array);
	g_signal_connect(cancelButton, "clicked", G_CALLBACK(destroySomething), adddialog);
	gtk_widget_show_all(adddialog);
	gtk_dialog_run(GTK_DIALOG(adddialog));
	gtk_widget_destroy(adddialog);
}

GtkWidget * initMessage
int main(int argc, char const *argv[])
{

	GtkWidget *inputGroupBox;
	GtkWidget *userListGroupBox;
	GtkWidget *currentUserGroupBox;
	GtkWidget *publicChannelGroupBox;
	GtkWidget *yournameLabel;
	GtkWidget *inputBox;
	GtkWidget *outputBox;
	GtkWidget *buttonBox;

	GtkWidget *addButton;
	GtkWidget *delButton;
	GtkWidget *editButton;
	GtkWidget *infoButton;
	GtkWidget *sendButton;
	GtkWidget *chat_output_scrolling;
	GtkWidget *user_list_scrolling;

	// Khoi tao GTK
	gtk_init(&argc, &argv);
	

	show_add_dialog();


	// Khoi tao cua so
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	//gtk_window_set_default_size(GTK_WINDOW(window), -1, -1);
	gtk_window_set_title(GTK_WINDOW(window), APP_TITLE);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	//Khoi tao nen
	frame = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), frame);
	gtk_widget_set_margin_bottom(frame, 5);
	gtk_widget_set_margin_right(frame, 5);
	
	//Khoi tao GroupBox
	inputGroupBox = gtk_frame_new(TEXT_INPUT_LABEL);
	set_size(inputGroupBox, 450, 60);
	set_pos(inputGroupBox, 120, 250);

	// Khoi tao List user groupbox
	userListGroupBox = gtk_frame_new(ONLINE_LIST_LABEL);
	set_size(userListGroupBox, 115, 186);
	set_pos(userListGroupBox, 5, 124);

	currentUserGroupBox = gtk_frame_new(YOU);
	set_size(currentUserGroupBox, 115, 54);
	set_pos(currentUserGroupBox, 5, 4);
	yournameLabel = gtk_label_new( you);
	gtk_container_add(GTK_CONTAINER(currentUserGroupBox), yournameLabel);


	publicChannelGroupBox = gtk_frame_new(PUBLIC);
	set_size(publicChannelGroupBox, 115, 54);
	set_pos(publicChannelGroupBox, 5, 64);
	//Khoi tao hop chua cac nut
	buttonBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	//Khoi tao thanh keo truot cho chatArea
	user_list_scrolling = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(user_list_scrolling), buttonBox);

	gtk_container_add(GTK_CONTAINER(userListGroupBox), user_list_scrolling);
	// set_size(buttonBox, 100, 250);
	// set_pos(buttonBox, 10, 10);

	//khoi tao hop chua chatArea hien thi nghia
	outputBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	set_size(outputBox, 450, 250);
	set_pos(outputBox, 120, 10);

	//Khoi tao hop chua o tim kiem va nut tim kiem
	inputBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add(GTK_CONTAINER(inputGroupBox), inputBox);

	//Khoi tao cac nut chuc nang
	editButton = gtk_button_new_with_label("Sửa từ");
	gtk_container_add(GTK_CONTAINER(publicChannelGroupBox), editButton);
	delButton = gtk_button_new_with_label("Xóa từ");
	addButton = gtk_button_new_with_label("Thêm từ");
	infoButton = gtk_button_new_with_label("Thông tin");

	sendButton = gtk_button_new_with_label(SEND_LABEL);
	gtk_widget_set_margin_bottom(sendButton, 5);
	gtk_widget_set_margin_top(sendButton, 0);

	//Khoi tao o nhap va chatArea hien thi nghia
	messageInput = gtk_entry_new();
	set_size(messageInput, 388, 20);
	//
	chatArea = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(chatArea), GTK_WRAP_WORD_CHAR);//Chong tran be ngang

	//Khoi tao thanh keo truot cho chatArea
	chat_output_scrolling = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(chat_output_scrolling), chatArea);


	//Dat cac thanh phan vao hop tuong ung
	// gtk_box_pack_start(GTK_BOX(buttonBox), addButton, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(buttonBox), editButton, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(buttonBox), delButton, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(buttonBox), infoButton, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(buttonBox), gtk_button_new_with_label("Thông tin"), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(buttonBox), gtk_button_new_with_label("Thông tin"), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(buttonBox), gtk_button_new_with_label("Thông tin"), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(buttonBox), gtk_button_new_with_label("Thông tin"), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(buttonBox), gtk_button_new_with_label("Thông tin"), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(buttonBox), gtk_button_new_with_label("Thông tin"), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(buttonBox), gtk_button_new_with_label("Thông tin"), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(buttonBox), gtk_button_new_with_label("Thông tin"), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(inputBox), messageInput, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(inputBox), sendButton, TRUE, TRUE, 5);

	gtk_box_pack_start(GTK_BOX(outputBox), chat_output_scrolling, TRUE, TRUE, 2);

	gtk_widget_show_all(window);


	g_signal_connect(window, "destroy", G_CALLBACK (gtk_main_quit), NULL);//Ket thuc chuong trinh khi dong cua so chinh

	//Gan su kien cho cac nut
	// g_signal_connect(sendButton, "clicked", G_CALLBACK(find_in_dict), NULL);
	// g_signal_connect(addButton, "clicked", G_CALLBACK(Show_add_dialog), NULL);
	// g_signal_connect(editButton, "clicked", G_CALLBACK(Show_edit_dialog), NULL);
	// g_signal_connect(delButton, "clicked", G_CALLBACK(Show_delete_dialog), NULL);
	// g_signal_connect(infoButton, "clicked", G_CALLBACK(Show_about_dialog), about_dialog);
	// g_signal_connect(messageInput, "key-press-event", G_CALLBACK(on_key_down), NULL);
	// g_signal_connect(messageInput, "activate", G_CALLBACK(find_in_dict), NULL);

	gtk_main();

	return 0;
}