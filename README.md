# Time Tracker
### About
This is a time tracker much like work. Create a project, start and label a timer, end the timer and save to disk, then view time spent on the project. That's all this does.

This project came about since I found myself too focused during programming sessions to the point where I would neglect proper time management and skip meals or not have enough time to do things I already mentally planned on doing. I use this daily to track overall time spent on various tasks whenever I am at my computer.

First and foremost this is a hobby project meant to assist me; far from a polished product meant for other users. If you are reading this, attempts have been made to allow to allow intuitive usage for you, but I have not spent much time developing the interface and explaining all that you may need to know or any side effects that may arise.

Secondly, this is a test bed to allow me to practice my software design skills and test new facets of C++ and programming in general on a larger project than a hello-world program. If you have any tips or questions, please inform me. I am always open to learn. 

### Install
Prerequisites:
- Requires CMake
- Currently as of **version 1.6.3 only Linux is supported**

Steps:
1. Clone the project
2. Navigate into the `Time-Tracker` directory
3. Create build directory
4. Navigate into the `build` directory
5. Run CMake to build necessary build files
6. Create the executable binary

Once this is done, the executable binary will be located in the `Time-Tracker/bin` directory.

All together, the commands will be

```
$ git clone <URL>
$ cd Time-Tracker
$ mkdir build
$ cd build
$ cmake ..
$ make
```


### Usage
Currently as of **version 1.6.3 only Linux is supported**. 
- The default location to save project files to is `~/Documents/` which is where the program will create the directory `Time-Tracker`.
- The default RC file path is the home directory. The program will create the file `.timetracker-rc` which is akin to a `.bashrc` file. Commands entered here will be executed upon each startup. 

Once you are running the program and internal setup was successful, the following prompt will appear 

```
TIME TRACKER>
```

The command `help` will display further information about all commands and their usage.
