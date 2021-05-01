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
        
   }
   else if( argc > 2 ) {
        printf("Too many arguments supplied.\n");
   }
   else {
      printf("One argument expected.\n");
   }
}