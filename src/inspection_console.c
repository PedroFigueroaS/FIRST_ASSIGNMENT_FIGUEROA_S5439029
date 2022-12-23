#include "./../include/inspection_utilities.h"
#include <signal.h>
#define LEN 10
float ee_x = 0.0;
float ee_z = 0.0;

void exit_handler(int signo3)//Signal interruption to close the process
{
    if (signo3 == SIGQUIT)
    {
        printf("Signal received, exiting");
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }
}

void stop(int signo1)//Send an interruption to the command process: stop the hoist motion
{
    if (signo1 == SIGUSR1)
    {
        // printf("Signal received, exiting");
        fflush(stdout);
        char line[LEN];
        FILE *cmd = popen("pidof -s command", "r");//request the PID of the command console process
        fgets(line, LEN, cmd);
        long pid = 0;
        pid = strtoul(line, NULL, 10);

        kill(pid, SIGUSR1);
    }
}

void reset(int signo2)//Send an interruption to motorx and motorz processes: reset the position of the hoist
{
    if (signo2 == SIGUSR1)
    {
        // printf("Signal received, exiting");
        fflush(stdout);
        char line1[LEN];
        char line2[LEN];
        FILE *cmd1 = popen("pidof -s motor_x", "r");
        FILE *cmd2 = popen("pidof -s motor_z", "r");
        fgets(line1, LEN, cmd1);
        fgets(line2, LEN, cmd2);
        long pid1 = 0;
        long pid2 = 0;
        pid1 = strtoul(line1, NULL, 10);
        pid2 = strtoul(line2, NULL, 10);
        ee_x = 0.0;
        ee_z = 0.0;
        update_console_ui(&ee_x, &ee_z);
        kill(pid1, SIGUSR1);
        kill(pid2, SIGUSR1);
    }
}

void k_process(int signo2)//Send an interruption to the watchdog process: reset the count
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
    fp_i = fopen("inspection.log", "w");
    // declaration of pipes
    int fd;
    char *myfifo = "/tmp/myfifo5";//pipe address of the world process
    if(mkfifo(myfifo, 0666)!=0){
        perror("warning message:");
    }
    /*char *myfifo = "/tmp/inspectionpipe";
     if(mkfifo(myfifo, 0666)!=0){
        perror("fifo directory already exists\n");
    }*/
    char realpos[80];
    char format_string[80] = "%f,%f";
    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // End-effector coordinates

    // Initialize User Interface
    init_console_ui();

    // SIGNAL
    if (signal(SIGQUIT, exit_handler) == SIG_ERR)//Signal Handler to exit process
        printf("\ncan't catch SIGQUIT\n");
    if (signal(SIGUSR2, k_process) == SIG_ERR)//Signal Handler reset count in the watchdog
        printf("\ncan't catch SIGUSR2\n");
    // Infinite loop
    while (TRUE)
    {
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

                // STOP button pressed
                if (check_button_pressed(stp_button, &event))//stop button
                {
                    fprintf(fp_i, "Signal interruption: stop hoist...\n");
                    fflush(fp_i);
                    if (signal(SIGUSR1, stop) == SIG_ERR)//Signal Handler to stop the hoist motion
                        printf("\ncan't catch SIGINT\n");
                    // signal(SIGUSR1, Handle);
                    kill(getpid(), SIGUSR1);//Send USR1 signal to inspection console process
                    kill(getpid(), SIGUSR2);//Send USR2 signal to inspection console process
                }

                // RESET button pressed
                else if (check_button_pressed(rst_button, &event))//Reset button
                {
                    fprintf(fp_i, "Signal interruption: stop hoist, reset position...\n");
                    fflush(fp_i); 
                    if (signal(SIGUSR1, reset) == SIG_ERR)//Signal Handler to reset the hoist position
                        printf("\ncan't catch SIGINT\n");
                    kill(getpid(), SIGUSR1);
                    if (signal(SIGUSR1, stop) == SIG_ERR)//Signal Handler to stop the hoist motion
                        printf("\ncan't catch SIGINT\n");
  
                    kill(getpid(), SIGUSR1);//Send USR1 signal to inspection console process
                    kill(getpid(), SIGUSR2);//Send USR2 signal to inspection console process
                }
            }
        }

        // To be commented in final version...
        fd = open(myfifo, O_RDWR);//open world process pipe
        read(fd, realpos, 80);//read the positions 
        sscanf(realpos, format_string, &ee_x, &ee_z);
        close(fd);//close pipe
        usleep(40000);

        // Update UI
        update_console_ui(&ee_x, &ee_z);//update the position of the hoist in the window
    }

    // Terminate
    endwin();
    return 0;
}
