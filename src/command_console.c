#include "./../include/command_utilities.h"
#include <signal.h>
#define LEN 10
float velx, velz, vlimx, vlimz; // declare global variables

void exit_handler(int signo3)
{
    if (signo3 == SIGQUIT) // Signal interruption to close the process
    {
        
        printf("Signal received, exiting");
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }
}
void stop(int signo) // Signal interruption to stop the hoist motion
{
    if (signo == SIGUSR1)
    {
        // printf("Signal received, exiting");
        fflush(stdout);
        velx = 0.0;
        velz = 0.0;
    }
}

void k_process(int signo2) // Send a signal to the watchdog reset the count
{
    if (signo2 == SIGUSR2)
    {
        // printf("Signal received, exiting");
        fflush(stdout);
        char line[LEN];
        FILE *cmd = popen("pidof -s watchdog", "r");
        fgets(line, LEN, cmd);
        long pid = 0;
        pid = strtoul(line, NULL, 10);

        kill(pid, SIGUSR2);
    }
}

int main(int argc, char const *argv[])
{
    FILE *fp_i;
    fp_i = fopen("command.log", "w");
    int fd1, fd2;
    // pipe for motorx
    char *myfifo = "/tmp/myfifo"; // Pipe address for the motorx process
    if (mkfifo(myfifo, 0666) != 0)
    {
        perror("fifo directory already exists\n");
    }
    // pipe for motorz
    char *myfifo2 = "/tmp/myfifo2"; // Pipe address for the motorz process
    if (mkfifo(myfifo2, 0666) != 0)
    {
        perror("fifo directory already exists\n");
    }
    // pipe for motorx
    /*char *myfifo = "/tmp/mxpipe";
    if (mkfifo(myfifo, 0666) != 0)
    {
        perror("fifo directory already exists\n");
    }
    // pipe for motorz
    char *myfifo2 = "/tmp/mzpipe";
    if (mkfifo(myfifo2, 0666) != 0)
    {
        perror("fifo directory already exists\n");
    }*/
    velx = 0;
    velz = 0;
    vlimx = 2;
    vlimz = 2;
    char vel[20];
    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize User Interface
    init_console_ui();
    if (signal(SIGUSR1, stop) == SIG_ERR) // Signal Handler to stop the hoist
        printf("\ncan't catch SIGUSR1\n");
    if (signal(SIGUSR2, k_process) == SIG_ERR) // Signal Handler reset count in the watchdog
        printf("\ncan't catch SIGUSR2\n");
    if (signal(SIGQUIT, exit_handler) == SIG_ERR) // Signal Handler to exit process
        printf("\ncan't catch SIGQUIT\n");
    // Infinite loop
    while (TRUE)
    {
        fd1 = open(myfifo, O_WRONLY); // open motorx pipe
        fflush(stdout);
        sprintf(vel, "%f", velx);
        write(fd1, vel, strlen(vel) + 1); // write posx to the motorx process
        close(fd1);                       // close pipe
        fd2 = open(myfifo2, O_WRONLY);    // open motorz pipe
        fflush(stdout);
        sprintf(vel, "%f", velz);
        write(fd2, vel, strlen(vel) + 1); // write posz to the motorx process
        close(fd2);                       // close pipe
        usleep(10000);                    // wait for the current threat for an specific time

        // Get mouse/resize commands in non-blocking mode...
        int cmd = getch();

        // If user resizes screen, re-draw UI
        if (cmd == KEY_RESIZE)
        {
            if (first_resize)
            {
                first_resize = FALSE;
            }
            else
            {
                reset_console_ui();
            }
        }
        // Else if mouse has been pressed
        else if (cmd == KEY_MOUSE)
        {

            // Check which button has been pressed...
            if (getmouse(&event) == OK)
            {

                // Vx++ button pressed
                if (check_button_pressed(vx_incr_btn, &event))
                {

                    if (velx < 0)
                    {
                        velx = 0;
                    }

                    else
                    {
                        velx = velx + 0.25;

                        if (velx > vlimx)
                        {
                            velx = vlimx;
                        }
                    }
                    kill(getpid(), SIGUSR2); // Send USR2 signal to watchdog to reset count
                    fprintf(fp_i, "X velocity increase\n");
                    fflush(fp_i);
                    // signal(SIGUSR1, Handle);

                    /*fd1 = open(myfifo, O_WRONLY);
                    fflush(stdout);
                    sprintf(vel, "%f", velx);
                    write(fd1, vel, strlen(vel) + 1);
                    close(fd1);*/

                    /*mvprintw(LINES - 1, 1, "Horizontal Speed Increased");
                     refresh();
                     sleep(1);
                     for(int j = 0; j < COLS; j++) {
                         mvaddch(LINES - 1, j, ' ');
                     }*/
                }

                // Vx-- button pressed
                if (check_button_pressed(vx_decr_btn, &event))
                {

                    if (velx > 0)
                    {
                        velx = 0;
                    }
                    else
                    {
                        velx = velx - 0.25;
                        if (velx < -vlimx)
                        {
                            velx = -vlimx;
                        }
                    }
                    kill(getpid(), SIGUSR2); // Send USR2 signal to watchdog to reset count
                    fprintf(fp_i, "X velocity decrease\n");
                    fflush(fp_i);
                    /*fd1 = open(myfifo, O_WRONLY);
                    fflush(stdout);
                    sprintf(vel, "%f", velx);
                    write(fd1, vel, strlen(vel) + 1);
                    close(fd1);*/
                    /*
                    mvprintw(LINES - 1, 1, "Horizontal Speed Decreased");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }*/
                }

                // Vx stop button pressed
                if (check_button_pressed(vx_stp_button, &event))
                {

                    velx = 0;
                    kill(getpid(), SIGUSR2); // Send USR2 signal to watchdog to reset count
                     fprintf(fp_i, "X velocity stop\n");
                    fflush(fp_i);
                    /*fd1 = open(myfifo, O_WRONLY);
                    fflush(stdout);
                    sprintf(vel, "%f", velx);
                    write(fd1, vel, strlen(vel) + 1);
                    close(fd1);*/

                    /*    mvprintw(LINES - 1, 1, "Horizontal Motor Stopped");
                        refresh();
                        sleep(1);
                        for(int j = 0; j < COLS; j++) {
                            mvaddch(LINES - 1, j, ' ');

                        }*/
                }
                // Vz++ button pressed
                if (check_button_pressed(vz_incr_btn, &event))
                {
                    if (velz < 0)
                    {
                        velz = 0;
                    }

                    else
                    {
                        velz = velz + 0.25;

                        if (velz > vlimz)
                        {
                            velz = vlimz;
                        }
                    }
                    kill(getpid(), SIGUSR2); // Send USR2 signal to watchdog to reset count
                     fprintf(fp_i, "Z velocity increase\n");
                    fflush(fp_i);
                    /*fd2 = open(myfifo2, O_WRONLY);
                    fflush(stdout);
                    sprintf(vel, "%f", vely);
                    write(fd2, vel, strlen(vel) + 1);
                    close(fd2);*/
                }

                // Vz-- button pressed
                if (check_button_pressed(vz_decr_btn, &event))
                {
                    if (velz > 0)
                    {
                        velz = 0;
                    }
                    else
                    {
                        velz = velz - 0.25;
                        if (velz < -vlimz)
                        {
                            velz = -vlimz;
                        }
                    }
                    kill(getpid(), SIGUSR2); // Send USR2 signal to watchdog to reset count
                     fprintf(fp_i, "Z velocity decrease\n");
                    fflush(fp_i);
                    /*fd2 = open(myfifo2, O_WRONLY);
                    fflush(stdout);
                    sprintf(vel, "%f", vely);
                    write(fd2, vel, strlen(vel) + 1);
                    close(fd2);*/
                    /*mvprintw(LINES - 1, 1, "Vertical Speed Decreased");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }*/
                }

                // Vz stop button pressed
                if (check_button_pressed(vz_stp_button, &event))
                {

                    velz = 0;
                    kill(getpid(), SIGUSR2); // Send USR2 signal to watchdog to reset count
                     fprintf(fp_i, "Z velocity stop\n");
                    fflush(fp_i);
                    /*fd2 = open(myfifo2, O_WRONLY);
                    fflush(stdout);
                    sprintf(vel, "%f", vely);
                    write(fd2, vel, strlen(vel) + 1);
                    close(fd2);*/

                    /*mvprintw(LINES - 1, 1, "Vertical Motor Stopped");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }*/
                }
            }
        }

        refresh();
    }

    // Terminate
    endwin();
    return 0;
}
