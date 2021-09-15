#include <stdio.h>
#include <gtk/gtk.h>

extern  gchar *currentServerIP;
extern  gchar *currentUsername;
extern  gchar *currentPassword;
extern gchar *currentServerDomain;
extern gchar *screenResValue;

extern  gchar *ipToAdd;
extern  gchar *nameToAdd;

extern char *tofuResp;

extern GtkComboBoxText *serverField;
extern GtkWidget *wrongCredentialsMessage;
extern GtkEntry *usernameTextbox;
extern GtkEntry *passwordTextbox;
extern GtkComboBoxText *serverList;

extern GtkEntry *configIPTextbox;
extern GtkEntry *configNameTextbox;
extern GtkEntry *configScreenTextbox;

extern void closeThinPiManager (GtkWidget *wid, gpointer ptr);
extern void hideErrorMessage(GtkWidget *wid, gpointer ptr);
extern void getServerConfig();
extern void iniConfigBeta();
extern void openConnection(gchar *u, gchar *p, gchar *s);
extern void setUserInfo();
int checkForUpdates();
int SearchFile(char *name, char *str);
extern char openTOFUDialog();

//INI Returns
extern char* configName;
extern char* configServer;
extern char* configScreen;
extern int *configDrives;
extern int *configUSB;
extern int *configPrinters;
extern char *configDomain;
extern void getiniConfigBeta();

typedef struct
{
    int version;
    const char* type;
    const char* name;
    const char* ip;
    const int* usb;
    const int* printers;
    const int* drives;
    const char* res;
	const char* domain;
} configuration;

extern configuration tpsconfig;
