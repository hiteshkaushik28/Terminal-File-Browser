/*  
    AUTHOR: HITESH KAUSHIK 
    ROLL NO: 2018201057 
    COPYRIGHT (C) HITESH KAUSHIK PG-1
*/
#ifndef LISTING_H
#define LISTING_H
// Headers
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <bits/stdc++.h>
#include <pwd.h>
#include <grp.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <iomanip>
#include <vector>
#include <sys/ioctl.h>

using namespace std;

/* Global data structure for file names */
extern vector<string> files;
extern stack<string> enter;
extern stack<string> lft;
extern stack<string> rght;
extern vector<string> tokens;
extern struct winsize w;

/* Function Declarations */
void blank();
void debug();
void debug1();
void debug2();
void resetcursor();
int getdir(char *);
int left_enter();
int right_enter();
int dir_enter(int );
int backspace();
int command_mode();
void command_prompt();
int command_process();
void cp(char *,char *);
void err(char *, char *);
void renamefile(char [],char []);
void removefile(char []);
void createdir(char[],int);
void createfile(char[]);
#endif



