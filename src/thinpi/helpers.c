#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/thinpi.h"
#include <string.h>
#include "ini.h"

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
void managerInfo();

int numofcon;
configuration tparr[256];

configuration tpsconfig;

void LOG(char *str)
{
    printf("THINPI[*] - %s\n", str);
}

int tpexec(char *str, char *timeout)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        char *cmd = malloc(100);
        sprintf(cmd, "%s", str);
        int rv = execl(str, timeout, NULL);
        if (rv != 0)
        {
            LOG("tpexec timedout");
        }
    }
}

void closeThinPiManager(GtkWidget *wid, gpointer ptr)
{
    LOG("User exited thinpi forcefully");
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
    int index;
    index = gtk_combo_box_get_active(serverList);
    // printf("%d", index);
    currentServerIP = tparr[index].ip;
    currentServerDomain = tparr[index].domain;
    screenResValue = tparr[index].res;
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

void iniConfigBeta()
{
    managerInfo();
}

void managerInfo()
{

    ini_t *config = ini_load("/thinpi/config/thinpi.ini");

    const char *constr = ini_get(config, "thinpi_proto", "numcon");
    if (constr)
    {
        // printf("numofcon: %s\n", constr);
        numofcon = atoi(constr);
    }

    int i = 0;
    char tempcon[20];
    while (i < numofcon)
    {
        sprintf(tempcon, "connection%d", i);
        const char *name = ini_get(config, tempcon, "name");
        const char *ip = ini_get(config, tempcon, "ip");
        const char *usb = ini_get(config, tempcon, "usb");
        const char *printers = ini_get(config, tempcon, "printers");
        const char *drives = ini_get(config, tempcon, "drives");
        const char *res = ini_get(config, tempcon, "res");
        const char *domain = ini_get(config, tempcon, "domain");
        if (name)
        {
            tparr[i].name = name;
        }
        if (ip)
        {
            tparr[i].ip = ip;
        }
        if (usb)
        {
            tparr[i].usb = atoi(usb);
            // printf("usb: %d\n", tparr[i].usb);
        }
        if (printers)
        {
            tparr[i].printers = atoi(printers);
            // printf("printers: %d\n", tparr[i].printers);
        }
        if (drives)
        {
            tparr[i].drives = atoi(drives);
            // printf("drives: %d\n", tparr[i].drives);
        }
        if (res)
        {
            tparr[i].res = res;
        }
        if (domain)
        {
            tparr[i].domain = domain;
        }
        gtk_combo_box_text_append(serverList, NULL, tparr[i].name);
        i++;
    }
    gtk_combo_box_set_active(serverList, 0);
}
