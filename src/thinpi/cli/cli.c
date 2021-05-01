#include <stdio.h>
#include <stdlib.h>

void checkArg(char *sArg)
{
    printf("%s\n", sArg);
 
}

void main (int argc, char *argv[])
{
    system("figlet ThinPi");\
    //this is failing in this build.... no idea why the argv wont pass into the other if statments.....
    sleep(2);
    if( argc == 2 ) {
        if ( argv[2] == '-r' ) {
        printf("Reset all config files in the system\n");
        } else if (argv[1] == 'install') {
        char pkg[100];
        char cmd[100];

        printf("Package to install: \n");
        gets( pkg );
        sprintf(cmd, "sudo apt-get install %s", pkg);
        system(cmd);
        }
   }
   else if( argc > 2 ) {
        printf("Too many arguments supplied.\n");
   }
   else {
      printf("One argument expected.\n");
   }
}