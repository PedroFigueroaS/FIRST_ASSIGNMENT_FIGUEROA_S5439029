#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
void exit_handler(int signo)//Signal interruption to close the process
{
  if (signo == SIGQUIT)
  {
    printf("Signal received, exiting\n");
    fflush(stdout);
    exit(EXIT_SUCCESS);
  }
}

int spawn_k(const char *program, char *arg_list[])//function to spawn a process and open a console for it
{
  pid_t child_pid = fork();
  if (child_pid < 0)
  {
    perror("Error while forking...");
    return 1;
  }
  else if (child_pid != 0)
  {
    return child_pid;
  }
  else
  {
    if (execvp(program, arg_list) == 0);
    perror("Exec failed");
    return 1;
  }
}

int spawn_p(char *arg_list[])//function to spawn a process without opening console
{
  pid_t child_pid = fork();
  if (child_pid < 0)
  {
    perror("Error while forking...");
    return 1;
  }
  else if (child_pid != 0)
  {
    return child_pid;
  }
  else
  {
    if (execvp(arg_list[0],arg_list) == 0);
    perror("Exec failed");
    return 1;
  }
}

int main()
{
  FILE *fp_i;
  fp_i = fopen("master.log", "w");
  if (signal(SIGQUIT, exit_handler) == SIG_ERR)
    printf("\ncan't catch SIGINT\n");

  //argument list to call the executable of each process  
  char *arg_list_command[] = {"/usr/bin/konsole", "-e", "./bin/command", NULL}; 
  char *arg_list_motor_x[] = {"./bin/motor_x", NULL};
  char *arg_list_motor_z[] = {"./bin/motor_z", NULL};
  char *arg_list_world[] = {"./bin/world", NULL};
  char *arg_list_inspection[] = {"/usr/bin/konsole", "-e", "./bin/inspection", NULL};
  char *arg_list_watchdog[] = {"/usr/bin/konsole", "-e", "./bin/watchdog", NULL};

  //The spawn_k and spawn_p functions are called to execute the other processes

  pid_t pid_cmd = spawn_k("/usr/bin/konsole", arg_list_command);
  pid_t pid_mx = spawn_p(arg_list_motor_x);
  pid_t pid_mz = spawn_p(arg_list_motor_z);
  pid_t pid_world = spawn_p( arg_list_world);
  pid_t pid_insp = spawn_k("/usr/bin/konsole", arg_list_inspection);
  pid_t pid_wtd = spawn_k("/usr/bin/konsole", arg_list_watchdog);
  
  fprintf(fp_i, "command PID:%d\n",pid_cmd);
  fprintf(fp_i, "motorx PID:%d\n",pid_mx);
  fprintf(fp_i, "motorz PID:%d\n",pid_mz);
  fprintf(fp_i, "world PID:%d\n",pid_world);
  fprintf(fp_i, "inspection PID:%d\n",pid_insp);
  fprintf(fp_i, "watchdog PID:%d\n",pid_wtd);
  fflush(fp_i);
  
  
  int status;
  waitpid(pid_cmd, &status, 0);
  waitpid(pid_mx, &status, 0);
  waitpid(pid_mz, &status, 0);
  waitpid(pid_world, &status, 0);
  waitpid(pid_insp, &status, 0);
  waitpid(pid_wtd, &status, 0);

  


  printf("Main program exiting with status %d\n", status);
  return 0;
}
