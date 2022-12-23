#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
float posz = 0.0;

void exit_handler(int signo3)//Signal interruption to close the process
{
    if (signo3 == SIGQUIT)
    {
        printf("Signal received, exiting");
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }
}
void reset(int signo2)
{
    if (signo2 == SIGUSR1)//Signal interruption to reset the Z position to 0.0
    {
        // printf("Signal received, exiting");
        fflush(stdout);
        posz = 0.0;
    }
}

int main()
{
    FILE *fp_i;
    fp_i = fopen("motorz.log", "w");
    int fd1,fd2;

    char *myfifo = "/tmp/myfifo2";//Pipe address that comes from the command console
    //mkfifo(myfifo, 0666);
    char *myfifo2 = "/tmp/myfifo4";//Pipe address that goes to the world process
    if(mkfifo(myfifo2, 0666)!=0){
        perror("warning message:");
    }

    /*char *myfifo = "/tmp/mzpipe";
     if(mkfifo(myfifo, 0666)!=0){
        perror("fifo directory already exists\n");
    }
    char *myfifo2 = "/tmp/mztoworldpipe";
    if(mkfifo(myfifo2, 0666)!=0){
        perror("fifo directory already exists\n");
    }*/
    char vel[80];
    char pz[80];
    char format_string[80] = "%f";

    float lim_z;
    float velz, dt;

    velz = 0;
    dt = 0.5;
    lim_z = 9;

    /*
    Movement process
    */
    if (signal(SIGUSR1, reset) == SIG_ERR)//Signal Handler to reset position 
        printf("\ncan't catch SIGUSR1\n");
    if (signal(SIGQUIT, exit_handler) == SIG_ERR)//Signal Handler to exit process
        printf("\ncan't catch SIGQUIT\n");
    while (1)
    {
        // printf("vely: %f\n", vely);
        //

        fd2 = open(myfifo2, O_WRONLY);//open world process pipe
        sprintf(pz, "%f", posz);
        write(fd2, pz, strlen(pz) + 1);//write the position and sent it to the world process
        close(fd2);//close pipe
        //printf("posz: %f\n", posy);
        fprintf(fp_i, "posz:%f\n",posz);
        fflush(fp_i);
        fd1 = open(myfifo, O_RDONLY);//open command console pipe
        read(fd1, vel, 80);//Read the velocity value from the command console
        sscanf(vel, format_string, &velz);
        close(fd1);//close pipe

        posz = posz + dt * velz;
        if (posz > lim_z)
        {
            posz = lim_z;
        }
        // velx = velx - 0.25;
        if (posz < 0)
        {
            posz = 0.0;
        }
        usleep(500000);//wait for the current threat for an specific time
    }
    // Terminate
    // endwin();
    return 0;
}