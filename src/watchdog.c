#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#define LEN 10
int count = 0;

void kmaster_1(FILE *s)//Function to get the PID of an specific process
{
    char line[LEN];
    fgets(line, LEN, s);
    long pid = 0;
    pid = strtoul(line, NULL, 10);
    kill(pid, SIGQUIT);
}

void kmaster(int signo1) //Send a signal to kill the other process and to kill the watchdog
{
    if (signo1 == SIGQUIT)
    {

        // printf("Signal received, exiting");
        fflush(stdout);
        
      
        FILE *cmd1 = popen("pidof -s command", "r");
        kmaster_1(cmd1);
        FILE *cmd2 = popen("pidof -s motor_x", "r");
        kmaster_1(cmd2);
        FILE *cmd3 = popen("pidof -s motor_z", "r");
        kmaster_1(cmd3);
         FILE *cmd4 = popen("pidof -s world", "r");
        kmaster_1(cmd4);
         FILE *cmd5 = popen("pidof -s inspection", "r");
        kmaster_1(cmd5);
        FILE *cmd6 = popen("pidof -s master", "r");
        kmaster_1(cmd6);

        exit(EXIT_SUCCESS);
    }
}

void k_process(int signo2)//Signal interruption to reset the count of the watchdog
{
 
    if (signo2 == SIGUSR2)
    {   

        // printf("Signal received, exiting");
        fflush(stdout);
        count = 0;
    }
}

int main()
{
       FILE *fp_i;
    fp_i = fopen("watchdog.log", "w");
    if (signal(SIGUSR2, k_process) == SIG_ERR)//Signal Handler to reset the count
        printf("\ncan't catch SIGUSR2\n");
    if (signal(SIGQUIT, kmaster) == SIG_ERR)//Signal Handler to exit process
        printf("\ncan't catch SIGQUIT\n");

    while (1)
    {
        printf("counter: %d\n", count);
        count = count + 1;//Start to count every second. Only if the k_process signal interrupts, the count resets
        sleep(1);
        if (count==0){
        fprintf(fp_i, "Reset count...\n");
        fflush(fp_i);
        }

        if (count == 30)//after 30 seconds without interruptions, the process send a signal to exit all the processes, terminating the simulation
        {
            kill(getpid(), SIGQUIT);
        }
    }
    // Terminate
    //endwin();
    return 0;
}