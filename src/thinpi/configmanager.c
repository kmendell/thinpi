#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/thinpi.h"
#include "./ini/minIni.h"

#define sizearray(a) (sizeof(a) / sizeof((a)[0]))

const char inifile[] = "/thinpi/test.ini";
#define MAX 10

GtkEntry *configIPTextbox;
GtkEntry *configNameTextbox;
GtkEntry *configScreenTextbox;
GtkCheckButton *configUsbSelect;
GtkComboBoxText *configServerList;
GtkEntry *configDomainTextbox;
GtkCheckButton *configPrinterSelect;
GtkCheckButton *configHomeSelect;
GtkWidget *addButton;
GtkWidget *removeButton;

configuration arr_config[2];

gboolean usbActive;
gboolean printerActive;
gboolean homeActive;

void handle(GtkWidget *wid, gpointer ptr);
void checkHandle(GtkCheckButton *wid, gpointer ptr);
void listHandle(GtkComboBox *widget, gpointer user_data);
void addNewServer();

void main(int argc, char *argv[])
{

    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "/thinpi/Interface/configmanager.glade", NULL);

    GtkWindow *configWindow = (GtkWindow *)gtk_builder_get_object(builder, "configWindow");
    addButton = (GtkWidget *)gtk_builder_get_object(builder, "addButton");
    removeButton = (GtkWidget *)gtk_builder_get_object(builder, "removeButton");
    configServerList = (GtkComboBoxText *)gtk_builder_get_object(builder, "configServerList");
    configNameTextbox = (GtkEntry *)gtk_builder_get_object(builder, "serverNameTextbox");
    configIPTextbox = (GtkEntry *)gtk_builder_get_object(builder, "ipaddressTextbox");
    configDomainTextbox = (GtkEntry *)gtk_builder_get_object(builder, "domainNameTextbox");
    configUsbSelect = (GtkCheckButton *)gtk_builder_get_object(builder, "usbCheckbox");
    configPrinterSelect = (GtkCheckButton *)gtk_builder_get_object(builder, "printerCheckbox");
    configHomeSelect = (GtkCheckButton *)gtk_builder_get_object(builder, "homefolderCheckbox");
    configScreenTextbox = (GtkEntry *)gtk_builder_get_object(builder, "screenResTextbox");

    getiniConfigBeta();

    arr_config[0].name = configName;
    arr_config[0].res = configScreen;
    arr_config[0].ip = configServer;
    arr_config[0].domain = configDomain;
    arr_config[0].usb = configUSB;
    arr_config[0].printers = configPrinters;
    arr_config[0].drives = configDrives;

    gtk_entry_set_text(configNameTextbox, configName);
    gtk_entry_set_text(configIPTextbox, configServer);
    gtk_entry_set_text(configScreenTextbox, configScreen);
    gtk_entry_set_text(configDomainTextbox, configDomain);
    gtk_toggle_button_set_active(configUsbSelect, configUSB);
    gtk_toggle_button_set_active(configPrinterSelect, configPrinters);
    gtk_toggle_button_set_active(configHomeSelect, configDrives);
    gtk_combo_box_text_append(configServerList, NULL, configName);
    gtk_combo_box_text_append(configServerList, NULL, "<Add New>");
    gtk_combo_box_set_active(configServerList, 0);

    g_signal_connect(configWindow, "delete_event", G_CALLBACK(closeThinPiManager), NULL);
    g_signal_connect(addButton, "clicked", G_CALLBACK(handle), "addButton");
    g_signal_connect(removeButton, "clicked", G_CALLBACK(handle), "removeButton");
    g_signal_connect(configUsbSelect, "toggled", G_CALLBACK(checkHandle), "usb");
    g_signal_connect(configPrinterSelect, "toggled", G_CALLBACK(checkHandle), "printer");
    g_signal_connect(configHomeSelect, "toggled", G_CALLBACK(checkHandle), "home");
    g_signal_connect(configServerList, "changed", G_CALLBACK(listHandle), "server changed");

    gtk_window_present(configWindow);
    gtk_main();
}

void addNewServer()
{

    char str[100];
    char usb[100];
    char prin[100];
    char home[100];
    long n;
    int s, k;
    char section[50];
    sprintf(usb, "%d", usbActive);
    sprintf(prin, "%d", printerActive);
    sprintf(home, "%d", homeActive);

    n = ini_puts("Connection", "ip", gtk_entry_get_text(configIPTextbox), inifile);
    n = ini_puts("Connection", "name", gtk_entry_get_text(configNameTextbox), inifile);
    if (strcmp(gtk_entry_get_text(configScreenTextbox), "") == 0)
    {
        n = ini_puts("Connection", "res", "1920x1080", inifile);
    }
    else
    {
        n = ini_puts("Connection", "res", gtk_entry_get_text(configScreenTextbox), inifile);
    }
    n = ini_puts("Connection", "usb", usb, inifile);
    n = ini_puts("Connection", "printers", prin, inifile);
    n = ini_puts("Connection", "drives", home, inifile);
    n = ini_puts("Connection", "domain", gtk_entry_get_text(configDomainTextbox), inifile);

    printf("[THINPI] - Server Added\n");
}

void handle(GtkWidget *wid, gpointer ptr)
{
    if (strcmp(ptr, "addButton") == 0)
    {
        if (strcmp(gtk_button_get_label(addButton), "Add and Save") == 0)
        {
            addNewServer();
        }
        else
        {
            printf("Server Edited\n");
        }
    }
}

void checkHandle(GtkCheckButton *wid, gpointer ptr)
{
    gboolean active;
    g_object_get(wid, "active", &active, NULL);
    if (strcmp("usb", ptr) == 0)
    {
        usbActive = active;
    }
    else if (strcmp("home", ptr) == 0)
    {
        homeActive = active;
    }
    else if (strcmp("printer", ptr) == 0)
    {
        printerActive = active;
    }
}

void listHandle(GtkComboBox *widget, gpointer user_data)
{
    gchar *stext;
    stext = gtk_combo_box_text_get_active_text(configServerList);

    if (strcmp("<Add New>", stext) == 0)
    {
        gtk_button_set_label(addButton, "Add and Save");
        gtk_entry_set_text(configNameTextbox, "");
        gtk_entry_set_text(configIPTextbox, "");
        gtk_entry_set_text(configDomainTextbox, "");
        gtk_entry_set_text(configScreenTextbox, "");
        gtk_toggle_button_set_active(configUsbSelect, 0);
        gtk_toggle_button_set_active(configPrinterSelect, 0);
        gtk_toggle_button_set_active(configHomeSelect, 0);
    }
    else
    {

        size_t length = sizeof(arr_config) / sizeof(arr_config[0]);
        int i;
        for (i = 0; i < length - 1; i++)
        {
            if (strcmp(arr_config[i].name, stext) == 0)
            {
                gtk_button_set_label(addButton, "Edit and Save");
                gtk_entry_set_text(configNameTextbox, arr_config[i].name);
                gtk_entry_set_text(configIPTextbox, arr_config[i].ip);
                gtk_entry_set_text(configScreenTextbox, arr_config[i].res);
                gtk_entry_set_text(configDomainTextbox, arr_config[i].domain);
                gtk_toggle_button_set_active(configUsbSelect, arr_config[i].usb);
                gtk_toggle_button_set_active(configPrinterSelect, arr_config[i].printers);
                gtk_toggle_button_set_active(configHomeSelect, arr_config[i].drives);
            }
        }
    }
}