# ARP-Hoist-Assignment-Summary

The Assignment consists of the design, test, and deploy of an interactive simulation of a hoist that moves in two axes (X & Z axis), with two manual consoles that allow the user to control the velocity in each axis of the hoist, and to stop or return to its initial position.

The simulation involves 7 codes that are the processes required to control the project. These process are as follows:

## The master process
The main process of the simulation, calls the other processes and gives each one of them a PID.

In the master code, to call other processes, it is used a spawn function that receives a string that contains the address of the desired executable. Into the spawn function, it is used the fork command to create a child process for the executable and run it properly with the execvp command.

## The command console process
The command console is the first part of the simulation, which allows the user to send velocity data to the motor process.

The command console will always send a velocity value to each motor process, independently if one of the buttons is pushed/clicked, to avoid the blocking issue of the named pipes while reading the data sent. There are two float variables (velx,vely), which are the velocity data sent through pipes to the motor processes.

Because the code initializes with velx=0, and vely=0, the velocity sent to the motor process will be zero, thus the position will be also zero and will only increase or decrease if one of the buttons is pushed. 

Depending on the axis, each time a V+ or V- button is pressed, the velocity variable will increase or decrease 0.25 until they reach a max/min velocity of +/-2. If the stop button of each axis is pushed, the velocity value will become zero, stopping the movement of the hoist.

The command console also has three signal interruptions:

Exit handler: This interruption causes the program to end its execution, due to the task of the watchdog.

Stop: This interruption causes to change any value of velocity into 0, due to the stop/reset button of the inspection console.

k_process: This interruption is activated each time a button is pushed, and to send a signal to the watchdog.

## MotorX/Z processes

The MotorX/Z processes receive the information (velocity) from the command console and compute the position of the hoist. In the motor process, there are two named pipes created, one for reading the information that comes from the command console, and the other one to write this information to the inspection console.

The computed position also has limits, in the X-axis from 0 to 38, and the Z-axis from 0 to 9. 

Both motor processes have two signal interruptions:

Exit handler: This interruption causes the program to end its execution, due to the task of the watchdog.

reset: This interruption causes to return of the position in any motor process to 0

## World process

The World process reads the position computed from the motor processes and adds a percentage of error to obtain real measures of the position of the hoist, before sending this position to the inspection console. 

To read data from two different pipes at the same time, it is necessary to implement the select command. The value obtained from the select command will enter into the switch loop to evaluate three different situations: if the select value returns -1, there will be an error in the select command, if the returned value is 2, the two pipes from the motor processes write information, and it will have to randomly select one of them to read, and if the returned value is 1, only one pipe has information to read, and the system has to decide which one of them it is. 

After reading and adding the measured error to the positions, another pipe is opened to write a string of (posx,posz) and sent to the inspection console.

The world process has one signal interruption:

Exit handler: This interruption causes the program to end its execution, due to the task of the watchdog.

## Inspection Console

The inspection console receives from the world process a pipe that writes the desired coordinate of the hoist and proceeds to plot it in the window. Also, have two buttons that will send interruption signals. The S(Stop) button, when pushed will make the hoist stop in its current position, and wait to receive a change in the velocity. The R(Reset) button, when pushed will make the hoist return to the origin.

The inspection console has four signal interruptions:

Exit handler: This interruption causes the program to end its execution, due to the task of the watchdog.

stop: This interruption causes to send another interruption signal to the command console, to stop the hoist, changing any velocity value to 0

reset: This interruption causes to send another interruption signal to the motor X/Z process, to return the hoist to the origin, changing any position value to 0.

k_process: This interruption is activated each time a button is pushed, and to send a signal to the watchdog.

## Watchdog process

The watchdog process checks for the other process that involves command interaction (command console and inspection console). If after a certain time there is no interaction, the watchdog will kill and close the other process, ending the simulation

The watchdog process has two signal interruptions:

kmaster: This interruption will request the PID of the other process involved in the simulation, and will send to each one of them a signal to end their execution

k_process: This interruption causes the watchdog process to reset its counter each time a button is pushed.

For every process, a logfile is created to write the actions performed during the simulation
