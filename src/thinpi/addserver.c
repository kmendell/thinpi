#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/thinpi.h"

gchar *ipToAdd;
gchar *nameToAdd;
GtkEntry *ipaddressTextbox;
GtkEntry *displaynameTextbox;

int removeServer(GtkWidget *wid, gpointer ptr);

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
g_signal_connect (removeButton, "clicked", G_CALLBACK(removeServer), NULL);

gtk_window_present(configWindow);
gtk_main();

}


void removeAll(char * str, const char * toRemove)
{
    int i, j, stringLen, toRemoveLen;
    int found;

    stringLen   = strlen(str);      // Length of string
    toRemoveLen = strlen(toRemove); // Length of word to remove


    for(i=0; i <= stringLen - toRemoveLen; i++)
    {
        /* Match word with string */
        found = 1;
        for(j=0; j < toRemoveLen; j++)
        {
            if(str[i + j] != toRemove[j])
            {
                found = 0;
                break;
            }
        }

        /* If it is not a word */
        if(str[i + j] != ' ' && str[i + j] != '\t' && str[i + j] != '\n' && str[i + j] != '\0') 
        {
            found = 0;
        }

        /*
         * If word is found then shift all characters to left
         * and decrement the string length
         */
        if(found == 1)
        {
            for(j=i; j <= stringLen - toRemoveLen; j++)
            {
                str[j] = str[j + toRemoveLen];
            }

            stringLen = stringLen - toRemoveLen;

            // We will match next occurrence of word from current index.
            i--;
        }
    }
}


int removeServer(GtkWidget *wid, gpointer ptr)
{
    FILE * fPtr;
    FILE * fTemp;
    char path = malloc(100);
    
    char *toRemove = malloc(1000);
    char *buffer = malloc(1000);

    char *string = malloc(100);
    sprintf(toRemove, "%s:%s\n", gtk_entry_get_text (ipaddressTextbox), gtk_entry_get_text (displaynameTextbox));

    /* Input source file path path */
    path = "/thinpi/config/servers";

    //toRemove = string;


    /*  Open files */
    fPtr  = fopen("/thinpi/config/servers", "r");
    fTemp = fopen("/thinpi/config/delete.tmp", "w"); 

    /* fopen() return NULL if unable to open file in given mode. */
    if (fPtr == NULL || fTemp == NULL)
    {
        /* Unable to open file hence exit */
        printf("\nUnable to open file.\n");
        printf("Please check whether file exists and you have read/write privilege.\n");
        exit(EXIT_SUCCESS);
    }


    /*
     * Read line from source file and write to destination 
     * file after removing given word.
     */
    while ((fgets(buffer, BUFSIZ, fPtr)) != NULL)
    {
        // Remove all occurrence of word from current line
        removeAll(buffer, toRemove);

        // Write to temp file
        fputs(buffer, fTemp);
    }


    /* Close all files to release resource */
    fclose(fPtr);
    fclose(fTemp);


    /* Delete original source file */
    remove(path);

    /* Rename temp file as original file */
    rename("/thinpi/config/delete.tmp", path);


    printf("\nServer '%s' has been removed successfully.", toRemove);

    return 0;
}



