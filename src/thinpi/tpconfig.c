#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/thinpi.h"

gchar *ipToAdd;
gchar *nameToAdd;
GtkEntry *ipaddressTextbox;
GtkEntry *displaynameTextbox;
int dline = 0;

#define BUFFER_SIZE 1000

void rhandler(GtkWidget *wid, gpointer ptr);
void deleteLine(FILE *srcFile, FILE *tempFile, const int line);

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

	gtk_entry_set_text(ipaddressTextbox, "");
	gtk_entry_set_text(displaynameTextbox, "");
	
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
g_signal_connect (removeButton, "clicked", G_CALLBACK(rhandler), NULL);

gtk_window_present(configWindow);
gtk_main();

}

void rhandler(GtkWidget *wid, gpointer ptr) {
	
	   char *toRemove = malloc(1000);
	char *string = malloc(100);
    	sprintf(toRemove, "%s:%s\n", gtk_entry_get_text (ipaddressTextbox), gtk_entry_get_text (displaynameTextbox));
	
	SearchFile("/thinpi/config/servers", toRemove);
}


int SearchFile(char *name, char *str) {
	FILE *fp;
	int line = 1;
	int result = 0;
	char temp[256];

	if((fp = fopen(name, "r")) == NULL) {
		return(-1);
	}

	while(fgets(temp, 512, fp) != NULL) {
		if((strstr(temp, str)) != NULL) {
			printf("A Match Found on line: %d\n", line);
			dline = line;
			printf("\n%s\n", temp);
			result++;
		}
		line++;
	}

	if(result == 0) {
		printf("\nSorry server doesnt exsist\n");
	} else {
		int comp = delete();
		if(comp == 0) {
			gtk_entry_set_text(ipaddressTextbox, "");
			gtk_entry_set_text(displaynameTextbox, "");
		}
	}


	if(fp) {
		fclose(fp);
	}
	return(0);
}

int delete() {
    FILE *srcFile  = fopen("/thinpi/config/servers", "r");
    FILE *tempFile = fopen("delete-line.tmp", "w");


    /* Exit if file not opened successfully */
    if (srcFile == NULL || tempFile == NULL)
    {
        printf("Unable to open file.\n");
        printf("Please check you have read/write previleges.\n");

        return -1;
    }


    // Move src file pointer to beginning
    rewind(srcFile);

    // Delete given line from file.
    deleteLine(srcFile, tempFile, dline);


    /* Close all open files */
    fclose(srcFile);
    fclose(tempFile);


    /* Delete src file and rename temp file as src */
    remove("/thinpi/config/servers");
    rename("delete-line.tmp", "/thinpi/config/servers");
    return 0;
}

void deleteLine(FILE *srcFile, FILE *tempFile, const int line)
{
    char buffer[BUFFER_SIZE];
    int count = 1;

    while ((fgets(buffer, BUFFER_SIZE, srcFile)) != NULL)
    {
        /* If current line is not the line user wanted to remove */
        if (line != count)
            fputs(buffer, tempFile);

        count++;
    }
}



