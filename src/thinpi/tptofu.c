#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/thinpi.h"

char *tofuResp;

void yesHandle(GtkWidget *wid, gpointer ptr)
{
    LOG("User Accepted TOFU Agreement");
    tofuResp = "yes";
    printf("%s\n", tofuResp);
    closeThinPiManager(wid, ptr);
}

void noHandle(GtkWidget *wid, gpointer ptr)
{
    LOG("User declined TOFU Agreement");
    tofuResp = "no";
    printf("%s\n", tofuResp);
    closeThinPiManager(wid, ptr);
}

char openTOFUDialog()
{
    return tofuResp;
}

char main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new();

    gtk_builder_add_from_file(builder, "/thinpi/Interface/tofu.glade", NULL);

    GtkWindow *tofuWindow = (GtkWindow *)gtk_builder_get_object(builder, "tptofuDialog");
    GtkWidget *yesBtn = (GtkWidget *)gtk_builder_get_object(builder, "yesBtn");
    GtkWidget *noBtn = (GtkWidget *)gtk_builder_get_object(builder, "noBtn");

    LOG("TOFU Dialog Loaded");

    g_signal_connect(yesBtn, "clicked", G_CALLBACK(yesHandle), NULL);
    g_signal_connect(noBtn, "clicked", G_CALLBACK(noHandle), NULL);
    g_signal_connect(tofuWindow, "delete_event", G_CALLBACK(closeThinPiManager), NULL);

    gtk_window_present(tofuWindow);
    gtk_main();

    if (tofuResp == "yes")
    {
        return 0;
    }
    else
    {
        return -1;
    }
}