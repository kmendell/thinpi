#include <stdio.h>
#include <stdlib.h>

int help()
{
   printf("Usage: tpcli [options]\n");
   printf("\t-i package\n");
   printf("\t-r package\n");
   printf("\t-u updates the thinpi os system\n");
   printf("\t-ua runs apt-get update and upgrade \n");
   printf("\t-d defaults thinpi configuration\n");
   printf("\t--reboot\n");
   printf("\t--adduser user\n");
   printf("\t--deluser user\n");
   printf("\t--man-start\n");
   printf("\t--config-start\n");

   return 1;
}

int main(int argc, char **argv) {

    int i;

    for (i=0; i < argc; i++) {

        if (strcmp("-u", argv[i]) == 0) { // update thinpi filesystem

            printf("Updating ThinPi OS ...\n");

        } else if ((strcmp("-d", argv[i]) == 0)) { // default thinpi config

            printf("Defaulting ThinPi Configuration ...\n");

        } else if ((strcmp("-i", argv[i]) == 0)) { //d installed a apt package

            char *pkg[100];
            strcpy(pkg, argv[i+1]);

            printf("Installing Package %s...\n", pkg);
            char *cmd[100];
            sprintf(cmd, "sudo apt install %s", pkg);
            system(cmd);

        } else if ((strcmp("-r", argv[i]) == 0)) { // removes installed apt package

            char *pkg[100];
            strcpy(pkg, argv[i+1]);

            printf("Removing Package %s...\n", pkg);
            char *cmd[100];
            sprintf(cmd, "sudo apt remove %s", pkg);
            system(cmd);

        } else if ((strcmp("-ua", argv[i]) == 0)) { // updates apt packages

            system("sudo apt-get update");
            system("sudo apt-get upgrade -y");

        } else if ((strcmp("--reboot", argv[i]) == 0)) { // reboots the system

            printf("Rebooting the System ...\n");

        } else if ((strcmp("--adduser", argv[i]) == 0)) { // adds a linux user

            char *user[100];
            strcpy(user, argv[i+1]);

            char *cmd[100];
            sprintf(cmd, "sudo adduser %s", user);
            system(cmd);

        } else if ((strcmp("--deluser", argv[i]) == 0)) { //deletes a linux user

            char *user[100];
            strcpy(user, argv[i+1]);

            char *cmd[100];
            sprintf(cmd, "sudo deluser %s", user);
            system(cmd);

        } else if ((strcmp("--man-start", argv[i]) == 0)) { // starts thinpi connect manager

            system("/thinpi/thinpi-manager");

        } else if ((strcmp("--config-start", argv[i]) == 0)) { //starts thinpi config manager

            system("/thinpi/thinpi-manager");

        } else {
            help();
        }

    }

}
