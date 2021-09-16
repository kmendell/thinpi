#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/thinpi.h"

void handleClick(GtkWidget *wid, GtkEntry *pwordp)
{
	setUserInfo();
	openConnection(currentUsername, currentPassword, currentServerIP);
}

void openConfigManager(GtkWidget *wid, GtkEntry *pwordp)
{
	if (!fork())
	{
		// system("GDK_BACKEND=x11");
		int rv = system("/thinpi/thinpi-config");
		return rv;
	}
	else
	{
		return 0;
	}
}

void main(int argc, char *argv[])
{

	gtk_init(&argc, &argv);

	GtkBuilder *builder = gtk_builder_new();
	//update to fs all directorys will now /thinpi/*
	gtk_builder_add_from_file(builder, "/thinpi/Interface/connect-manager.glade", NULL);

	GtkWindow *window1 = (GtkWindow *)gtk_builder_get_object(builder, "thinpiMain");
	GtkWidget *btn = (GtkWidget *)gtk_builder_get_object(builder, "connect");
	GtkWidget *settingsButton = (GtkWidget *)gtk_builder_get_object(builder, "settingsButton");
	GtkWidget *openConfigButton = (GtkWidget *)gtk_builder_get_object(builder, "openConfigButton");
	usernameTextbox = (GtkEntry *)gtk_builder_get_object(builder, "uname");
	passwordTextbox = (GtkEntry *)gtk_builder_get_object(builder, "pword");
	serverList = (GtkComboBoxText *)gtk_builder_get_object(builder, "serverSelect");
	wrongCredentialsMessage = (GtkWidget *)gtk_builder_get_object(builder, "wrongLabel");
	copyrightLabel = (GtkWidget *)gtk_builder_get_object(builder, "copyrightLabel");

	gtk_label_set_label(copyrightLabel, "ThinPi v0.3.0-dev build 2021915 - Copyright KM Projects 2021");

	//getServerConfig();
	iniConfigBeta();

	g_signal_connect(btn, "clicked", G_CALLBACK(handleClick), NULL);
	g_signal_connect(openConfigButton, "clicked", G_CALLBACK(openConfigManager), NULL);
	g_signal_connect(settingsButton, "clicked", G_CALLBACK(openSettings), NULL);

	g_signal_connect(window1, "delete_event", G_CALLBACK(closeThinPiManager), NULL);
	g_signal_connect(usernameTextbox, "changed", G_CALLBACK(hideErrorMessage), NULL);
	g_signal_connect(passwordTextbox, "changed", G_CALLBACK(hideErrorMessage), NULL);

	gtk_window_fullscreen(window1);
	// gtk_window_fullscreen(GTK_WINDOW(gtk_widget_get_root_window(window1)));
	gtk_window_present(window1);
	gtk_main();
}
