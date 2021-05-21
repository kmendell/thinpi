#include <stdio.h>
#include <gtk/gtk.h>

extern  gchar *currentServerIP;
extern  gchar *currentUsername;
extern  gchar *currentPassword;
extern gchar *currentServerDomain;
extern gchar *screenResValue;

extern  gchar *ipToAdd;
extern  gchar *nameToAdd;

extern GtkComboBoxText *serverField;
extern GtkWidget *wrongCredentialsMessage;
extern GtkEntry *usernameTextbox;
extern GtkEntry *passwordTextbox;
extern GtkComboBoxText *serverList;

extern GtkEntry *ipaddressTextbox;
extern GtkEntry *displaynameTextbox;
extern GtkEntry *screenResTextbox;

extern void closeThinPiManager (GtkWidget *wid, gpointer ptr);
extern void hideErrorMessage(GtkWidget *wid, gpointer ptr);
extern void getServerConfig();
extern void openConnection(gchar *u, gchar *p, gchar *s);
extern void setUserInfo();
int checkForUpdates();
int SearchFile(char *name, char *str);
