/* Simple test program
 *
 *  gcc -o test test.c minIni.c
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "minIni.h"

#define sizearray(a)  (sizeof(a) / sizeof((a)[0]))

const char inifile[] = "/thinpi/test.ini";
const char inifile2[] = "testplain.ini";

int Callback(const char *section, const char *key, const char *value, void *userdata)
{
  (void)userdata; /* this parameter is not used in this example */
  printf("    [%s]\t%s=%s\n", section, key, value);
  return 1;
}

int main(void)
{
  char str[100];
  long n;
  int s, k;
  char section[50];

  /* string writing */
  n = ini_puts("Connection", "ip", "1.1.1.1", inifile);
  n = ini_puts("Connection", "name", "Example Server", inifile);
  n = ini_puts("Connection", "res", "1920x1080", inifile);
  n = ini_puts("Connection", "usb", "1", inifile);
  n = ini_puts("Connection", "printers", "0", inifile);
  n = ini_puts("Connection", "drives", "1", inifile);
  n = ini_puts("Connection", "domain", "domainname", inifile);
  /* ----- */

  return 0;
}

