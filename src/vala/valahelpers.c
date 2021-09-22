#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
// #include "../include/thinpi.h"
#include <string.h>
#include "../thinpi/ini.h"

typedef struct
{
    int version;
    const char *type;
    const char *name;
    const char *ip;
    const int *usb;
    const int *printers;
    const int *drives;
    const char *res;
    const char *domain;
} configuration;

int numofcon;

configuration tparr[256];

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
        // gtk_combo_box_text_append(serverList, NULL, tparr[i].name);
        i++;
    }
    // gtk_combo_box_set_active(serverList, 0);
}