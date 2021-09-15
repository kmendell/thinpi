#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/thinpi.h"
#define MAX 10

GtkEntry *configIPTextbox;
GtkEntry *configNameTextbox;
GtkEntry *configScreenTextbox;
GtkCheckButton *configUsbSelect;
GtkComboBoxText *configServerList;
GtkEntry *configDomainTextbox;

    configuration arr_config[2];

void handle(GtkWidget *wid, gpointer ptr);
void checkHandle(GtkCheckButton *wid, gpointer ptr);
void listHandle(GtkComboBox *widget, gpointer user_data);

void main(int argc, char *argv[])
{


	gtk_init(&argc, &argv);

	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "/thinpi/Interface/configmanager.glade", NULL);

	GtkWindow *configWindow = (GtkWindow *)gtk_builder_get_object(builder, "configWindow");
	GtkWidget *addButton = (GtkWidget *)gtk_builder_get_object(builder, "addButton");
	GtkWidget *removeButton = (GtkWidget *)gtk_builder_get_object(builder, "removeButton");
    configServerList = (GtkComboBoxText *)gtk_builder_get_object(builder, "configServerList");
	configNameTextbox = (GtkEntry *)gtk_builder_get_object(builder, "serverNameTextbox");
	configIPTextbox = (GtkEntry *)gtk_builder_get_object(builder, "ipaddressTextbox");
    configDomainTextbox = (GtkEntry *)gtk_builder_get_object(builder, "domainNameTextbox");
    configUsbSelect = (GtkCheckButton *)gtk_builder_get_object(builder, "usbCheckbox");
    GtkCheckButton *configPrinterSelect = (GtkCheckButton *)gtk_builder_get_object(builder, "printerCheckbox");
    GtkCheckButton *configHomeSelect = (GtkCheckButton *)gtk_builder_get_object(builder, "homefolderCheckbox");
	configScreenTextbox = (GtkEntry *)gtk_builder_get_object(builder, "screenResTextbox");

    getiniConfigBeta();
    printf("%s, %s, %s\n", configServer, configName, configScreen);

    arr_config[0].name = configName;
    arr_config[0].res = configScreen;
    arr_config[0].ip = configServer;
    arr_config[0].domain = configDomain;


    // printf("%s, %s, %s\n", arr_config[0].name, arr_config[0].ip, arr_config[0].res);

    gtk_entry_set_text(configNameTextbox, configName);
    gtk_entry_set_text(configIPTextbox, configServer);
    gtk_entry_set_text(configScreenTextbox, configScreen);
    gtk_entry_set_text(configDomainTextbox, configDomain);
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

void handle(GtkWidget *wid, gpointer ptr)
{
    g_print ("button event: %s\n", ptr);
}

void checkHandle(GtkCheckButton *wid, gpointer ptr)
{
    gboolean active;
    g_object_get(wid, "active", &active, NULL);     
    g_print ("%s value: %d\n", ptr, active);
}


void listHandle(GtkComboBox *widget, gpointer user_data) {
    gchar *stext;
    stext = gtk_combo_box_text_get_active_text(configServerList);
 

    if (strcmp("<Add New>", stext) == 0) {
        gtk_entry_set_text(configNameTextbox, "");
        gtk_entry_set_text(configIPTextbox, "");
        gtk_entry_set_text(configScreenTextbox, "");
    } else {

        size_t length = sizeof(arr_config)/sizeof(arr_config[0]);   
        int i;
        for(i = 0; i < length - 1; i++) {
            if (strcmp(arr_config[i].name, stext) == 0) {
                gtk_entry_set_text(configNameTextbox, arr_config[i].name);
                gtk_entry_set_text(configIPTextbox, arr_config[i].ip);
                gtk_entry_set_text(configScreenTextbox, arr_config[i].res);
            }
        }   

        // if (strcmp(arr_config[0].name, stext) == 0)

    }
}