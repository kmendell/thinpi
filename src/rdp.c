#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "../include/thinpi.h"

void openConnection(gchar *u, gchar *p, gchar *s)
{	
	char *cmd = malloc(100);
	sprintf(cmd, "xfreerdp +gfx-thin-client /cert-ignore /cert-tofu /size:1920x1080 /f /gdi:hw /smart-sizing:1920x1080 /u:'%s' /p:'%s' /v:%s", u, p, s);
	int rv = system(cmd);
	printf("%s\n", cmd);
	if(rv == 0 || rv == 2816) {
		gtk_entry_set_text(usernameTextbox, "");
		gtk_entry_set_text(passwordTextbox, "");
	} else if(rv != 0) {
		printf("Wrong Username or password\n");
		gtk_widget_show(wrongCredentialsMessage);
	}
	
}