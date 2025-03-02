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
1. Clone the project, navigate to it, and make the build directory
```
git clone <URL>
cd Time-Tracker
mkdir build
```
2. Run CMake to build the project
```
cmake --build build/
```
### Usage
Currently **only Linux is supported**. 
- The default location to save project files to is `~/Documents/` which is where the program will create the directory `Time-Tracker`.
- The default RC file path is `~/.config`. The program will create the file `.timetracker-rc` which is akin to a `.bashrc` file. Commands entered here will be executed upon each startup. 

Once you are running the program and internal setup was successful, the following prompt will appear 

```
TIME TRACKER>
```

The command `help` will display further information about all commands and their usage.

### Development and Debug Settings
To access the debug settings, create the debug option for CMake as follows
```
cmake -DDEBUG=ON build/
```
For the debug version, you should change the loctation of the save directory. To do so, in the `src/Framework.cpp` source file, change the value of the `projectsDirectory` string in both of the following functions: `deteremineSaveDirectory()` and `determineRCPath()`. This will change where the rc file and save directory are expected to be.
