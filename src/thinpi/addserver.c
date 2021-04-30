#include "../gtk-3.0/gtk/gtk.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/thinpi.h"

gchar *ipToAdd;
gchar *nameToAdd;
GtkEntry *ipaddressTextbox;
GtkEntry *displaynameTextbox;

void addNewServer(GtkWidget *wid, gpointer ptr)
{
	ipToAdd = gtk_entry_get_text (ipaddressTextbox);
	nameToAdd = gtk_entry_get_text (displaynameTextbox);
	char *string = malloc(100);
    FILE* file = fopen("/thinpi/config/servers", "a"); 
	sprintf(string, "%s:%s\n", ipToAdd, nameToAdd);
	printf("%s\n", string);
    fputs(string, file);
	
    

    fclose(file);
	
}


void main (int argc, char *argv[])
{

gtk_init (&argc, &argv);

GtkBuilder *builder = gtk_builder_new(); 
gtk_builder_add_from_file(builder, "/thinpi/Interface/addserver.glade", NULL); 

GtkWindow *configWindow = (GtkWindow *) gtk_builder_get_object (builder, "configWindow"); 
GtkWidget *addButton = (GtkWidget *) gtk_builder_get_object (builder, "addButton");
GtkWidget *removeButton = (GtkWidget *) gtk_builder_get_object (builder, "removeButton"); 
displaynameTextbox = (GtkEntry *) gtk_builder_get_object (builder, "displaynameTextbox"); 
ipaddressTextbox = (GtkEntry *) gtk_builder_get_object (builder, "ipaddressTextbox"); 

g_signal_connect (configWindow, "delete_event", G_CALLBACK(closeThinPiManager), NULL);
g_signal_connect (addButton, "clicked", G_CALLBACK(addNewServer), NULL);

gtk_window_present(configWindow);
gtk_main();

}


