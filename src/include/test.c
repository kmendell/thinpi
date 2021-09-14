#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ini.h"

typedef struct
{
    int version;
    const char* type;
    const char* name;
    const char* ip;
    const char* usb;
    const char* printers;
    const char* drives;
    const char* res;
} configuration;

configuration tpsconfig;

static int handler(void* connection, const char* section, const char* name, const char* value)
{
    configuration* pconfig = (configuration*)connection;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("thinpi_proto", "version")) {
        pconfig->version = atoi(value);
    } else if (MATCH("connection", "name")) {
        pconfig->name = strdup(value);
    } else if (MATCH("connection", "ip")) {
        pconfig->ip = strdup(value);
    } else if (MATCH("connection", "usb")) {
        pconfig->usb = strdup(value);
    } else if (MATCH("connection", "printers")) {
        pconfig->printers = strdup(value);
    } else if (MATCH("connection", "drives")) {
        pconfig->drives = strdup(value);
    } else if (MATCH("connection", "res")) {
        pconfig->res = strdup(value);
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

void test() {
	
	printf("%s\n", tpsconfig.name);
}

int main(int argc, char* argv[])
{
	
	configuration config;

    if (ini_parse("/thinpi/config/thinpi.ini", handler, &tpsconfig) < 0) {
        printf("Can't load 'thinpi.ini'\n");
        return 1;
    }
    printf("Config loaded from 'thinpi.ini': version=%d, name=%s, ip=%s, usb=%s, printers=%s, drives=%s, res=%s\n",
        tpsconfig.version, tpsconfig.name, tpsconfig.ip, tpsconfig.usb, tpsconfig.printers, tpsconfig.drives, tpsconfig.res);
		test();
    return 1;
}


