#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/thinpi.h"
#include "../include/minIni.h"
#include "ini.h"

#define sizearray(a) (sizeof(a) / sizeof((a)[0]))

const char inifile[] = "/thinpi/config/thinpi.ini";
const char editinifile[] = "/thinpi/config/thinpi.ini";
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

int numofcon;
configuration tparr[256];

gboolean usbActive;
gboolean printerActive;
gboolean homeActive;

void handle(GtkWidget *wid, gpointer ptr);
void checkHandle(GtkCheckButton *wid, gpointer ptr);
void listHandle(GtkComboBox *widget, gpointer user_data);
void addNewServer();
void editServer();
void iniGet();

void iniGet()
{

    ini_t *config = ini_load("/thinpi/config/thinpi.ini");

    const char *constr = ini_get(config, "thinpi_proto", "numcon");
    if (constr)
    {
        printf("numofcon: %s\n", constr);
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
            printf("name: %s\n", tparr[i].name);
        }
        if (ip)
        {
            tparr[i].ip = ip;
            printf("ip: %s\n", tparr[i].ip);
        }
        if (usb)
        {
            tparr[i].usb = atoi(usb);
            printf("usb: %d\n", tparr[i].usb);
        }
        if (printers)
        {
            tparr[i].printers = atoi(printers);
            printf("printers: %d\n", tparr[i].printers);
        }
        if (drives)
        {
            tparr[i].drives = atoi(drives);
            printf("drives: %d\n", tparr[i].drives);
        }
        if (res)
        {
            tparr[i].res = res;
            printf("res: %s\n", tparr[i].res);
        }
        if (domain)
        {
            tparr[i].domain = domain;
            printf("domain: %s\n", tparr[i].domain);
        }
        i++;
    }
}

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

    // getiniConfigBeta();
    iniGet();
    LOG("Config Manager Loaded");

    gtk_entry_set_text(configNameTextbox, tparr[0].name);
    gtk_entry_set_text(configIPTextbox, tparr[0].ip);
    gtk_entry_set_text(configScreenTextbox, tparr[0].res);
    gtk_entry_set_text(configDomainTextbox, tparr[0].domain);
    gtk_toggle_button_set_active(configUsbSelect, tparr[0].usb);
    gtk_toggle_button_set_active(configPrinterSelect, tparr[0].printers);
    gtk_toggle_button_set_active(configHomeSelect, tparr[0].drives);
    int i;
    for (i = 0; i < numofcon; i++)
    {
        gtk_combo_box_text_append(configServerList, NULL, tparr[i].name);
    }
    gtk_combo_box_text_append(configServerList, NULL, "<Add New>");
    gtk_combo_box_set_active(configServerList, 0);

    g_signal_connect(configWindow, "destroy", G_CALLBACK(gtk_window_close), NULL);
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
    char tmpcon[100];
    char usb[100];
    char prin[100];
    char home[100];
    int numcpy;
    long n;
    int s, k;
    char section[50];
    sprintf(usb, "%d", usbActive);
    sprintf(prin, "%d", printerActive);
    sprintf(home, "%d", homeActive);
    numcpy = numofcon + 1;
    sprintf(str, "connection%d", numofcon);
    sprintf(tmpcon, "%d", numcpy);

    n = ini_puts("thinpi_proto", "numcon", tmpcon, inifile);

    n = ini_puts(str, "ip", gtk_entry_get_text(configIPTextbox), inifile);
    n = ini_puts(str, "name", gtk_entry_get_text(configNameTextbox), inifile);
    if (strcmp(gtk_entry_get_text(configScreenTextbox), "") == 0)
    {
        n = ini_puts(str, "res", "1920x1080", inifile);
    }
    else
    {
        n = ini_puts(str, "res", gtk_entry_get_text(configScreenTextbox), inifile);
    }
    n = ini_puts(str, "usb", usb, inifile);
    n = ini_puts(str, "printers", prin, inifile);
    n = ini_puts(str, "drives", home, inifile);
    n = ini_puts(str, "domain", gtk_entry_get_text(configDomainTextbox), inifile);

    printf("[THINPI] - Server Added\n");
}

void editServer()
{

    char str[100];
    char usb[100];
    char prin[100];
    char home[100];
    long n;
    char section[50];
    sprintf(usb, "%d", usbActive);
    sprintf(prin, "%d", printerActive);
    sprintf(home, "%d", homeActive);

    gchar *stext;
    stext = gtk_combo_box_text_get_active_text(configServerList);
    int i;

    for (i = 0; i < numofcon; i++)
    {
        if (strcmp(tparr[i].name, stext) == 0)
        {
            sprintf(str, "connection%d", i);

            n = ini_puts(str, "ip", gtk_entry_get_text(configIPTextbox), editinifile);
            n = ini_puts(str, "name", gtk_entry_get_text(configNameTextbox), editinifile);
            if (strcmp(gtk_entry_get_text(configScreenTextbox), "") == 0)
            {
                n = ini_puts(str, "res", "1920x1080", editinifile);
            }
            else
            {
                n = ini_puts(str, "res", gtk_entry_get_text(configScreenTextbox), editinifile);
            }
            n = ini_puts(str, "usb", usb, editinifile);
            n = ini_puts(str, "printers", prin, editinifile);
            n = ini_puts(str, "drives", home, editinifile);
            n = ini_puts(str, "domain", gtk_entry_get_text(configDomainTextbox), editinifile);
        }
    }

    printf("[THINPI] - Server Edit Complete\n");
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
            editServer();
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

        size_t length = sizeof(tparr) / sizeof(tparr[0]);
        int i;
        for (i = 0; i < numofcon; i++)
        {
            if (strcmp(tparr[i].name, stext) == 0)
            {
                gtk_button_set_label(addButton, "Edit and Save");
                gtk_entry_set_text(configNameTextbox, tparr[i].name);
                gtk_entry_set_text(configIPTextbox, tparr[i].ip);
                gtk_entry_set_text(configScreenTextbox, tparr[i].res);
                gtk_entry_set_text(configDomainTextbox, tparr[i].domain);
                gtk_toggle_button_set_active(configUsbSelect, tparr[i].usb);
                gtk_toggle_button_set_active(configPrinterSelect, tparr[i].printers);
                gtk_toggle_button_set_active(configHomeSelect, tparr[i].drives);
            }
        }
    }
}