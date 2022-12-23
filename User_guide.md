# ARP-Hoist-Assignment-User Guide to Compile and run the project
GITHUB LINK OF THE PROJECT: https://github.com/PedroFigueroaS/FIRST_ASSIGNMENT_FIGUEROA_S5439029.git
In particular, the repository is organized as follows:
- The `src` folder contains the source code for the Command, Inspection, Master, Motor X, Motor Z, and the Watchdog
- The `include` folder contains all the data structures and methods used within the ncurses framework to build the two GUIs. 
- The `bin` folder is where the executable files of the codes in the src files are stored

## ncurses installation
To install the ncurses library, simply open a terminal and type the following command:
```console
sudo apt-get install libncurses-dev
```

## Compiling the project

In the main file, there is a shell executable called build.sh. This executable is run as follows:

```console
./build.sh
```
the content of this executable is the following code lines:
```console
#! /usr/bin/bash
gcc ./src/inspection_console.c -lncurses -lm -o ./bin/inspection
gcc ./src/command_console.c -lncurses -o ./bin/command
gcc ./src/motor_x.c -o ./bin/motor_x
gcc ./src/motor_z.c -o ./bin/motor_z
gcc ./src/world.c -o ./bin/world
gcc ./src/watchdog.c -o ./bin/watchdog
gcc ./src/master.c -o ./bin/master
```
This shell code allows to compile all the processes involved in the simulation and also is a more efficient way to make modifications.

## Running the project

In the same address of the shell executable to compile the project, there is the other executable that allows running the project called run_program.sh. In a terminal, type the next command

```console
./run_program.sh
```
the content of this executable is the following code lines:

```console
konsole  -e ./bin/master
```
This shell code allows running the master code of the project, which executes the other process involved in the project.

## Troubleshooting after launching the run executable

It is possible that after launching the run_program.sh for the first time, the motor x or motor z process crashes and compromises the simulation by not moving the hoist in the windows. In any case, follow the next steps in a terminal

1. run the ./build.sh command first, to compile and create the executables
2. run the ./run_program.sh command after that, to run the simulation
3. In case there is a crash problem or the simulation doesnt move the hoist, run again the ./run_program.sh command



