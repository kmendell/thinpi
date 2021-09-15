#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/thinpi.h"
#include <string.h>
#include "../include/ini.h"

gchar *currentServerIP;
gchar *currentServerDomain;
gchar *currentUsername;
gchar *currentPassword;
gchar *screenResValue;

GtkWidget *wrongCredentialsMessage;
GtkEntry *usernameTextbox;
GtkEntry *passwordTextbox;
GtkComboBoxText *serverList;
GtkWidget *copyrightLabel;

char *configName;
char *configServer;
char *configScreen;
int *configDrives;
int *configUSB;
int *configPrinters;
char *configDomain;

configuration tpsconfig;

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
    currentServerIP = configServer;
    //currentServerDomain = strtok(NULL, ":");
    screenResValue = configScreen;
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

static int handler(void *connection, const char *section, const char *name,
                   const char *value)
{
    configuration *pconfig = (configuration *)connection;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("thinpi_proto", "version"))
    {
        pconfig->version = atoi(value);
    }
    else if (MATCH("connection", "name"))
    {
        pconfig->name = strdup(value);
    }
    else if (MATCH("connection", "ip"))
    {
        pconfig->ip = strdup(value);
    }
    else if (MATCH("connection", "usb"))
    {
        pconfig->usb = atoi(value);
    }
    else if (MATCH("connection", "printers"))
    {
        pconfig->printers = atoi(value);
    }
    else if (MATCH("connection", "drives"))
    {
        pconfig->drives = atoi(value);
    }
    else if (MATCH("connection", "domain"))
    {
        pconfig->domain = strdup(value);
    }
    else if (MATCH("connection", "res"))
    {
        pconfig->res = strdup(value);
    }
    else
    {
        return 0; /* unknown section/name, error */
    }
    return 1;
}

void iniConfigBeta()
{

    configuration config;

    if (ini_parse("/thinpi/config/thinpi.ini", handler, &tpsconfig) < 0)
    {
        printf("Can't load 'thinpi.ini'\n");
        return 1;
    }
    printf("Config loaded from 'thinpi.ini'\n");
    configServer = tpsconfig.ip;
    configName = tpsconfig.name;
    configScreen = tpsconfig.res;
    gtk_combo_box_text_append(serverList, NULL, configName);
}

void getiniConfigBeta()
{

    configuration config;

    if (ini_parse("/thinpi/config/thinpi.ini", handler, &tpsconfig) < 0)
    {
        printf("Can't load 'thinpi.ini'\n");
        return 1;
    }
    printf("Config loaded from 'thinpi.ini'\n");
    configServer = tpsconfig.ip;
    configName = tpsconfig.name;
    configScreen = tpsconfig.res;
    configDomain = tpsconfig.domain;
    configUSB = tpsconfig.usb;
    configPrinters = tpsconfig.printers;
    configDrives = tpsconfig.drives;
}
