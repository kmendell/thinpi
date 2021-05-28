#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/thinpi.h"

void handleClick(GtkWidget *wid, GtkEntry *pwordp)
{
	setUserInfo();
	openConnection(currentUsername, currentPassword, currentServerIP);
	
}




void main (int argc, char *argv[])
{

if (argc == 2 ) {

	if (argv[1] == "-manager") {

		printf("manager launced\n");

	}

}

gtk_init (&argc, &argv);

GtkBuilder *builder = gtk_builder_new(); 
//update to fs all directorys will now /thinpi/*
gtk_builder_add_from_file(builder, "/thinpi/Interface/connect-manager.glade", NULL); 

GtkWindow *window1 = (GtkWindow *) gtk_builder_get_object (builder, "thinpiMain"); 
GtkWidget *btn = (GtkWidget *) gtk_builder_get_object (builder, "connect"); 
usernameTextbox = (GtkEntry *) gtk_builder_get_object (builder, "uname");
passwordTextbox= (GtkEntry *) gtk_builder_get_object (builder, "pword");
serverList = (GtkComboBoxText *) gtk_builder_get_object (builder, "serverSelect");
wrongCredentialsMessage = (GtkWidget *) gtk_builder_get_object (builder, "wrongLabel");

getServerConfig();

g_signal_connect (btn, "clicked", G_CALLBACK(handleClick), NULL);
	
 g_signal_connect (window1, "delete_event", G_CALLBACK(closeThinPiManager),
 NULL);
 g_signal_connect (usernameTextbox, "changed", G_CALLBACK (hideErrorMessage),
 NULL);
  g_signal_connect (passwordTextbox, "changed", G_CALLBACK (hideErrorMessage),
 NULL);


gtk_window_fullscreen(window1);
gtk_window_present(window1);
gtk_main();

}

