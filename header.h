#ifndef __Heder_H__
#define __Heder_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <stddef.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <ctype.h>
#include <pthread.h>  
#include <math.h>

//#include <GL/glut.h>
#include <sys/msg.h>
#include <stdbool.h>

// Define the struct for an ant
typedef struct {
int x;
int y;
int direction;
int speed;
int pheromone;
int eating;
} Ant;

// Define the struct for a food piece
typedef struct {
int x;
int y;
int amount;
} Food;

#endif
