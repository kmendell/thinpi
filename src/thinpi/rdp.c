#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include "../include/thinpi.h"

void openConnection(gchar *u, gchar *p, gchar *s)
{
	char *cmd = malloc(100);
	sprintf(cmd, "tprdp %s -p %s -u %s -z -x b -g %s -f -v", s, p, u, screenResValue);
	int rv = system(cmd);
	LOG(cmd);
	// printf("%d\n", rv);
	// int rv = execvp("tprdp", cmd);
	if (rv == 2816)
	{
		gtk_entry_set_text(usernameTextbox, "");
		gtk_entry_set_text(passwordTextbox, "");
		gtk_widget_hide(wrongCredentialsMessage);
	}
	else if (rv != 12 || rv != 11 || rv != 2 | rv != 1)
	{
		LOG("An Error Occured Please Contact your Administrator\n");
		// gtk_label_set_label(wrongCredentialsMessage, "")
		gtk_widget_show(wrongCredentialsMessage);
	}
	free(cmd);
}
