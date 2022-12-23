#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
float posx = 0.0;
float posz = 0.0;

void exit_handler(int signo3) // Signal interruption to close the process
{
    if (signo3 == SIGQUIT)
    {
        printf("Signal received, exiting");
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }
}

int generate_random(int l, int r) // function to obtain a random value between two values
{                                 // this will generate random number in range l and r
    int rand_num = (rand() % (r - l + 1)) + l;
    return rand_num;
}

int main()
{
    FILE *fp_i;
    fp_i = fopen("world.log", "w");
    char *myfifo = "/tmp/myfifo3"; // Pipe address that comes from the motor_x process
    // mkfifo(myfifo, 0666);
    char *myfifo2 = "/tmp/myfifo4"; // Pipe address that comes from the motor_z process
    // mkfifo(myfifo2, 0666);
    char *myfifo3 = "/tmp/myfifo5"; // Pipe address that goes to the inspection process
    if (mkfifo(myfifo3, 0666) != 0)
    {
        perror("warning message:");
    }
    /*char *myfifo = "/tmp/mxtoworldpipe";
     if(mkfifo(myfifo, 0666)!=0){
        perror("fifo directory already exists\n");
    }
    char *myfifo2 = "/tmp/mztoworldpipe";
     if(mkfifo(myfifo2, 0666)!=0){
        perror("fifo directory already exists\n");
    }
    char *myfifo3 = "/tmp/inspectionpipe";
    if(mkfifo(myfifo3, 0666)!=0){
        perror("fifo directory already exists\n");
    }*/
    char vel[80];
    char px[80];
    char pz[80];
    char realpos[80];
    char format_string[80] = "%f";
    char format_string_2[80] = "%f,%f";

    posx = 0.0;
    posz = 0.0;

    /*
    Movement process
    */
    if (signal(SIGQUIT, exit_handler) == SIG_ERR) // Signal Handler to exit process
        printf("\ncan't catch SIGQUIT\n");
    while (1)
    {
        fd_set fds;
        struct timeval tv;
        int maxfd;
        int res;
        int retval;
        // char buf[256];
        int fd1 = open(myfifo, O_RDWR);  // open motorx pipe
        int fd2 = open(myfifo2, O_RDWR); // open motorz pipe
        int fd3;
        FD_ZERO(&fds);     // Clear FD set for select
        FD_SET(fd1, &fds); // Set bit of the fd1 in fdset
        FD_SET(fd2, &fds); // Set bit of the fd2 in fdset
        tv.tv_sec = 0.1;
        tv.tv_usec = 0;
        maxfd = fd1 > fd2 ? fd1 : fd2; // Compares the returned values of the fd1 and fd2

        retval = select(maxfd + 1, &fds, NULL, NULL, NULL); // returns which file descriptor is ready for reading
        // printf("retval:%d\n",retval);
        switch (retval)
        {
        case -1: // returns and error
            perror("select()");
            return -1;

        case 2: // two pipes are ready for reading, choose randomly one of them
            // printf("There are two pipes with info to be read. Choosing one randomnly...\n");
            fflush(stdout);
            int rand_number = generate_random(0, 1);
            if (rand_number == 0)
            {
                fprintf(fp_i, "two pipes available: reading motorx pipe\n");
                fflush(fp_i);
                read(fd1, px, 80);
                sscanf(px, format_string, &posx);
                close(fd1);
                close(fd2);
            }
            else
            {
                fprintf(fp_i, "two pipes available: reading motorz pipe\n");
                fflush(fp_i);
                read(fd2, pz, 80);
                sscanf(pz, format_string, &posz);
                close(fd1);
                close(fd2);
            }
            break;

        case 1: // one pipe is ready for reading, check which one it is
            // printf("There is one pipe with info to be read.\n");
            fflush(stdout);
            if (FD_ISSET(fd1, &fds) != 0)
            {
                // printf("motorx pipe,\n");
                fprintf(fp_i, "motorx pipe ready for reading\n");
                fflush(fp_i);
                fflush(stdout);
                res = read(fd1, px, 80);
                sscanf(px, format_string, &posx);
                close(fd1);
                close(fd2);
            }
            else if (FD_ISSET(fd2, &fds) != 0)
            {
                // printf("motorz pipe\n");
                fprintf(fp_i, "motorz pipe ready for reading\n");
                fflush(fp_i);
                fflush(stdout);
                res = read(fd2, pz, 80);
                sscanf(pz, format_string, &posz);
                close(fd1);
                close(fd2);
            }
            break;
        }
        int rand_error = generate_random(0, 1); // adds measure error to obtain real world positions
        if (rand_error == 0)
        {
            posx = posx + 0.005;
            posz = posz + 0.005;
        }
        else
        {
            posx = posx - 0.005;
            posz = posz - 0.005;
        }

        printf("posx:%f,posz%f\n", posx, posz);
        fd3 = open(myfifo3, O_RDWR); // open inspection console pipe
        sprintf(realpos, "%f,%f", posx, posz);
        write(fd3, realpos, strlen(realpos) + 1); // write an string of two positions and send it to the inspection console
        close(fd3);                               // close pipe
        usleep(40000);
    }
    // Terminate
    // endwin();
    return 0;
}