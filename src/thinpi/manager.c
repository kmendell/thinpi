#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../include/thinpi.h"

#define TIME_LIMIT 5

GtkWidget *btn;

void handleClick(GtkWidget *wid, GtkEntry *pwordp)
{
	gtk_button_set_label(btn, "Connecting...");
	setUserInfo();
	openConnection(currentUsername, currentPassword, currentServerIP);
	gtk_button_set_label(btn, "Connect");
}

void openConfigManager(GtkWidget *wid, GtkEntry *pwordp)
{
	tpexec("/thinpi/thinpi-config", "");
}

void main(int argc, char *argv[])
{

	gtk_init(&argc, &argv);

	GtkBuilder *builder = gtk_builder_new();
	//update to fs all directorys will now /thinpi/*
	gtk_builder_add_from_file(builder, "/thinpi/Interface/connect-manager.glade", NULL);

	GtkWindow *window1 = (GtkWindow *)gtk_builder_get_object(builder, "thinpiMain");
	btn = (GtkWidget *)gtk_builder_get_object(builder, "connect");
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
	LOG("Connection Manager Loaded");

	connectWidget(btn, "clicked", handleClick, NULL);
	connectWidget(openConfigButton, "clicked", openConfigManager, NULL);
	connectWidget(settingsButton, "clicked", openSettings, NULL);
	connectWidget(window1, "delete-event", closeThinPiManager, NULL);
	connectWidget(usernameTextbox, "changed", hideErrorMessage, NULL);
	connectWidget(passwordTextbox, "changed", hideErrorMessage, NULL);

	gtk_button_set_label(openConfigButton, "Config Manager");

	gtk_window_fullscreen(window1);
	// gtk_window_fullscreen(GTK_WINDOW(gtk_widget_get_root_window(window1)));
	gtk_window_present(window1);
	gtk_main();
}
