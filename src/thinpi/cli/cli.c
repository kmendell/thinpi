#include <stdio.h>
#include <stdlib.h>

void checkArg(char *sArg)
{
    if (sArg == "reset") {
        printf("Reset all config files in the system\n");
    } else if (sArg == "install") {
        printf("Package to install: \n");
    } else {
        printf("Unknown Argument\n");
    }
}

void main (int argc, char *argv[])
{
    system("clear");
    system("figlet ThinPi");
    if( argc == 2 ) {
      printf("The argument supplied is %s\n", argv[1]);
      checkArg(argv[1]);
   }
   else if( argc > 2 ) {
        printf("Too many arguments supplied.\n");
   }
   else {
      printf("One argument expected.\n");
   }
}