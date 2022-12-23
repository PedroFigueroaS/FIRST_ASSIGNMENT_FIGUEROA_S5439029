#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
float posx = 0.0;

void exit_handler(int signo3)//Signal interruption to close the process
{
    if (signo3 == SIGQUIT)
    {
        printf("Signal received, exiting");
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }
}



void reset(int signo2)//Signal interruption to reset the X position to 0.0
{
    if (signo2 == SIGUSR1){
        fflush(stdout);
        posx=0.0;
    }
}



int main()
{
    FILE *fp_i;
    fp_i = fopen("motorx.log", "w");
    int fd1,fd2;
    char *myfifo = "/tmp/myfifo";//Pipe address that comes from the command console
    //mkfifo(myfifo, 0666);
    char *myfifo2 = "/tmp/myfifo3";//Pipe address that goes to the world process
    if(mkfifo(myfifo2, 0666)!=0){
        perror("warning message:");
    }
    /*char *myfifo = "/tmp/mxpipe";
     if(mkfifo(myfifo, 0666)!=0){
        perror("fifo directory already exists\n");
    }
    char *myfifo2 = "/tmp/mxtoworldpipe";
    if(mkfifo(myfifo2, 0666)!=0){
        perror("fifo directory already exists\n");
    }*/


    char vel[80];
    char px[80];
    char format_string[80] = "%f";

    float posy, lim_x;
    float velx, dt;

    velx = 0.0;
    dt = 0.5;
    lim_x = 38;
    float vx = 0;

   if (signal(SIGUSR1, reset) == SIG_ERR)//Signal Handler to reset position 
        printf("\ncan't catch SIGUSR1\n"); 
    if (signal(SIGQUIT, exit_handler) == SIG_ERR)//Signal Handler to exit process
        printf("\ncan't catch SIGQUIT\n");      
    while (1)
    {

        
        fd2 = open(myfifo2, O_WRONLY);//open world process pipe
        sprintf(px, "%f", posx);
        write(fd2, px, strlen(px) + 1);//write the position and sent it to the world process
        close(fd2);//close pipe
        fprintf(fp_i, "posx:%f\n",posx);
        fflush(fp_i);
        //printf("posx: %f\n", posx);
        fd1 = open(myfifo, O_RDONLY);//open command console pipe
        read(fd1, vel, 80);//Read the velocity value from the command console
        sscanf(vel, format_string, &velx);
        close(fd1);//close pipe

        posx = posx + dt * velx;
        if (posx > lim_x)
        {
            posx = lim_x;
        }
        if (posx < 0.0)
        {
            posx = 0.0;
        }
        usleep(500000);//wait for the current threat for an specific time
    }
    // Terminate
    //endwin();
    return 0;
}