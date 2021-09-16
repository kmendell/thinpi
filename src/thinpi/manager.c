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

void alarm_handler(int);
void child_handler(int);

int timeout = 0;
int child_done = 0;

GtkWidget *btn;

void child_handler(int sig)
{
	child_done = 1;
}

void alarm_handler(int sig)
{
	timeout = 1;
}

void handleClick(GtkWidget *wid, GtkEntry *pwordp)
{
	gtk_button_set_label(btn, "Connecting...");
	// sleep(1);
	setUserInfo();

	pid_t pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		exit(1);
	}
	else if (pid == 0)
	{

		// child process
		// char *args[] = {currentServerIP, "-p", currentPassword, "-u", currentUsername, "-z", "-x", "b", "-d", currentServerDomain, "-g", screenResValue, "-f", "-v"};
		// int rv = execvp("tprdp", args);
		// execl("./r.out", "r.out", NULL);
		openConnection(currentUsername, currentPassword, currentServerIP);
		perror("exec failed");
		exit(1);
	}

	signal(SIGALRM, alarm_handler);
	signal(SIGCHLD, child_handler);

	alarm(TIME_LIMIT); // install an alarm to be fired after TIME_LIMIT
	pause();

	if (timeout)
	{
		printf("alarm triggered\n");
		gtk_button_set_label(btn, "Connect");
		int result = waitpid(pid, NULL, WNOHANG);
		if (result == 0)
		{
			// child still running, so kill it
			printf("killing child\n");
			kill(pid, 9);
			wait(NULL);
		}
		else
		{
			gtk_button_set_label(btn, "Connect");
			printf("alarm triggered, but child finished normally\n");
		}
	}
	else if (child_done)
	{
		printf("child finished normally\n");
		gtk_button_set_label(btn, "Connect");
		wait(NULL);
	}
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

	gtk_window_fullscreen(window1);
	// gtk_window_fullscreen(GTK_WINDOW(gtk_widget_get_root_window(window1)));
	gtk_window_present(window1);
	gtk_main();
}
