#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/thinpi.h"

void openSettings(GtkWidget *wid, gpointer ptr)
{
    GtkBuilder *builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "/thinpi/Interface/settings.glade", NULL);

    GtkWindow *setingsWindow = (GtkWindow *)gtk_builder_get_object(builder, "settingsWindow");
    GtkNotebook *menuBar = (GtkNotebook *)gtk_builder_get_object(builder, "settingMenuBar");
    GtkButton *closeThinPiButton = (GtkButton *)gtk_builder_get_object(builder, "settingsOtherCloseButton");

    g_signal_connect(setingsWindow, "destroy", G_CALLBACK(gtk_window_close), NULL);
    g_signal_connect(closeThinPiButton, "clicked", G_CALLBACK(closeThinPiManager), NULL);

    gtk_window_present(setingsWindow);
}