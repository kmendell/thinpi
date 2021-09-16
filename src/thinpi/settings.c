#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/thinpi.h"

// void main(int argc, char *argv[])
// {

//     gtk_init(&argc, &argv);

//     // gtk_main();
// }

void openSettings(GtkWidget *wid, gpointer ptr)
{
    GtkBuilder *builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "/thinpi/Interface/settings.glade", NULL);

    GtkWindow *setingsWindow = (GtkWindow *)gtk_builder_get_object(builder, "settingsWindow");

    g_signal_connect(setingsWindow, "destroy", G_CALLBACK(gtk_window_close), NULL);

    gtk_window_present(setingsWindow);
}