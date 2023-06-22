#ifndef __CONSTANTS__
#define __CONSTANTS__

#define MAX_ANT 100
#define maxFood 100 // Max num of food pieces

static int antsNum = 0;                   // number of ants
static int placeFoodAmount = 0;           // time amount to place a piece of food
static int smellFoodDistance = 0;         // distance that the ant can smell the food
static int callAntsDistance = 0;          // the distance from the ant that smelled the food that other ants can notice
static int pheromoneSensitivityQuant = 0; // the pherom quantity to notice the ant call
static int foodPortionPercent = 0;        // percentage for the food portion that an ant can eat
static int simulationTime = 0;
static int xSpeed, ySpeed;

int num_ant = 0;
int num_food = 0;
int choices[] = {0, 45, 90, 135, 180, 225, 270, 315};
Ant ants[MAX_ANT];
Food foods[maxFood];
pthread_t antsThreads[MAX_ANT], foodThreads[maxFood];
pthread_mutex_t ant_initialize_mutex = PTHREAD_MUTEX_INITIALIZER,
                food_initialize_mutex = PTHREAD_MUTEX_INITIALIZER,
                ant_eating_mutex = PTHREAD_MUTEX_INITIALIZER,
                ant_updating_mutex = PTHREAD_MUTEX_INITIALIZER;

// define colors
enum colors
{
    BLACK = 1,
    BLUE,
    GREEN,
    CYAN,
    RED,
    WHITE,
    YELLOW,
    MAGENTA
};

#endif
