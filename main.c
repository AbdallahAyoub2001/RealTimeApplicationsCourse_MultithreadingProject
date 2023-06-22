#include "header.h"
#include "constants.h"
#include "functions.h"

void main(int argc, char *argv[])
{
    readConstants("arguments.txt"); // Read constants from args file
    srand((unsigned)getpid());
    antsCreation();

    // Start the simulation
    int time = 0;

    while (time < simulationTime)
    {

        if (time < simulationTime - 5 && time % 2 == 0)
        { // segmentation fault is gotten, edit the time of creating food.

            pthread_create(&foodThreads[num_food],
                           NULL,
                           (void *)init_food,
                           NULL);
            pthread_join(foodThreads[num_food], NULL);
        }

        update_ants(); // to update ants positions, and check if any can smell food near it.

        // Sleep for 1 second
        usleep(1000000);

        // Increase the time
        time++;
    }
}
