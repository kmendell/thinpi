#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/thinpi.h"

gchar *currentServerIP;
gchar *currentServerDomain;
gchar *currentUsername;
gchar *currentPassword;
gchar *screenResValue;

GtkWidget *wrongCredentialsMessage;
GtkEntry *usernameTextbox;
GtkEntry *passwordTextbox;
GtkComboBoxText *serverList;

void closeThinPiManager(GtkWidget *wid, gpointer ptr)
{
	gtk_main_quit();
}

void hideErrorMessage(GtkWidget *wid, gpointer ptr)
{

	gtk_widget_hide(wrongCredentialsMessage);
}

int checkForUpdates()
{
	system("/usr/bin/tpupdate");
}

void setUserInfo()
{

	currentUsername = gtk_entry_get_text(usernameTextbox);
	currentPassword = gtk_entry_get_text(passwordTextbox);
	currentServerIP = strtok(gtk_combo_box_text_get_active_text(serverList), ":");
	currentServerDomain = strtok(NULL, ":");
	screenResValue = strtok(NULL, "\n");
	printf("%s\n", screenResValue);
}

void getServerConfig()
{
	int nlines = 0;
	FILE *file = fopen("/thinpi/config/servers", "r");
	char line[256];

	while (fgets(line, sizeof(line), file))
	{
		nlines++;
		strtok(line, "\n");
		gtk_combo_box_text_append(serverList, NULL, line);
	}

	fclose(file);
}
