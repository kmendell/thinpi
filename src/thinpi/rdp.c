#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include "../include/thinpi.h"

void openConnection(gchar *u, gchar *p, gchar *s)
{
	char *cmd = malloc(100);
	char *args[] = {s, "-p", p, "-u", u, "-z", "-x", "b", "-d", currentServerDomain, "-g", screenResValue, "-f", "-v"};
	sprintf(cmd, "%s -p %s -u %s -z -x b -d %s -g %s -f -v", s, p, u, currentServerDomain, screenResValue);
	// int rv = system(cmd);
	int rv = execvp("tprdp", args);
	if (rv == 12 || rv == 11 || rv == 2 | rv == 1)
	{
		gtk_entry_set_text(usernameTextbox, "");
		gtk_entry_set_text(passwordTextbox, "");
	}
	else if (rv != 12 || rv != 11 || rv != 2 | rv != 1)
	{
		LOG("An Error Occured Please Contact your Administrator\n");
		gtk_widget_show(wrongCredentialsMessage);
	}
	free(cmd);
}
