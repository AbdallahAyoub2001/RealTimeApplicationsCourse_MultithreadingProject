#ifndef __FUNCTIONS__
#define __FUNCTIONS__

#include "header.h"
#include "constants.h"

void readConstants(char *filename)
{
    char line[200];
    char label[50];

    FILE *file;

    file = fopen(filename, "r");

    if (file == NULL)
    {
        perror("The argument file doesn't exist\n");
        exit(-2);
    }

    char separator[] = "=";

    while (fgets(line, sizeof(line), file) != NULL)
    {

        char *str = strtok(line, separator);
        strncpy(label, str, sizeof(label));
        str = strtok(NULL, separator);

        if (strcmp(label, "ants") == 0)
        {
            antsNum = atoi(str);
        }
        else if (strcmp(label, "placeFoodAmount") == 0)
        {
            placeFoodAmount = atoi(str);
        }
        else if (strcmp(label, "smellFoodDistance") == 0)
        {
            smellFoodDistance = atoi(str);
        }
        else if (strcmp(label, "callAntsDistance") == 0)
        {
            callAntsDistance = atoi(str);
        }
        else if (strcmp(label, "pheromoneSensitivityQuant") == 0)
        {
            pheromoneSensitivityQuant = atoi(str);
        }
        else if (strcmp(label, "foodPortionPercent") == 0)
        {
            foodPortionPercent = atoi(str);
        }
        else if (strcmp(label, "simulationTime") == 0)
        {
            simulationTime = atoi(str);
        }
    }

    fclose(file);
}

// Function to initialize the ants
void init_ant()
{
    pthread_mutex_lock(&ant_initialize_mutex);
    ants[num_ant].x = rand() % 100;
    ants[num_ant].y = rand() % 100;
    ants[num_ant].direction = choices[rand() % 8];
    ants[num_ant].speed = rand() % 10 + 1;
    ants[num_ant].pheromone = 0;
    ants[num_ant].eating = 0;

    printf("Ant #%d: x=%d, y=%d\n dir=%d, speed=%d, pheremone=%d, and eating=%d\n", num_ant, ants[num_ant].x, ants[num_ant].y, ants[num_ant].direction, ants[num_ant].speed, ants[num_ant].pheromone, ants[num_ant].eating);
    num_ant++;
    pthread_mutex_unlock(&ant_initialize_mutex);
}

// Function to initialize the food pieces
void init_food()
{
    pthread_mutex_lock(&food_initialize_mutex);
    foods[num_food].x = rand() % 100;
    foods[num_food].y = rand() % 100;
    foods[num_food].amount = 100;
    printf("$$$$ Piece of food #%d with x=%d, y=%d, and amount=%d\n", num_food, foods[num_food].x, foods[num_food].y, foods[num_food].amount);
    num_food++;
    pthread_mutex_unlock(&food_initialize_mutex);
}

// Function to check if an ant can smell food
int can_smell_food(Ant ant, Food food)
{
    int distance = sqrt(((ant.x - food.x) * (ant.x - food.x)) + ((ant.y - food.y) * (ant.y - food.y)));
    // printf("--Distance is: %d\n", distance);
    return distance <= smellFoodDistance;
}

// Function to check if two ants are within social distance
int can_smell_pheromone(Ant ant1, Ant ant2)
{
    int distance = sqrt((ant1.x - ant2.x) * (ant1.x - ant2.x) + (ant1.y - ant2.y) * (ant1.y - ant2.y));
    if (distance == 0)
        return -1;
    return distance <= callAntsDistance;
}

int is_effected_pheromone(Ant ant1, Ant ant2)
{
    int distance = sqrt((ant1.x - ant2.x) * (ant1.x - ant2.x) + (ant1.y - ant2.y) * (ant1.y - ant2.y));
    if (distance == 0 || ant2.pheromone < pheromoneSensitivityQuant)
        return -1;
    return distance < callAntsDistance / 2;
}

// Function to update the position of an ant
void update_ant(Ant *ant)
{
    // Check if the ant can smell food
    int xDist = 9999, yDist = 9999;
    int angle, dx, dy;
    printf("*** Check The distance for ant------------------------------------------------\n");
    for (int i = 0; i < num_food; i++)
    {

        if (foods[i].amount > 0 && can_smell_food(*ant, foods[i]))
        {
            printf("-----1. Ant is smelling food piece #%d\n", i);
            // Release pheromone
            ant->pheromone = 1;
            // pthread_mutex_lock(&ant_updating_mutex);
            for (int k = 0; k < antsNum; k++)
            {
                if (ants[k].pheromone != 1 && can_smell_pheromone(ants[k], *ant))
                {
                    printf("----###----Ant #%d is smelling pheromone and heading to the food piece #%d\n", k, i);
                    dx = foods[i].x - ants[k].x;
                    dy = foods[i].y - ants[k].y;
                    int distance = sqrt((ants[k].x - foods[i].x) * (ants[k].x - foods[i].x) + (ants[k].y - foods[i].y) * (ants[k].y - foods[i].y));
                    if (ants[k].pheromone < 1 - (distance / 100))
                    {
                        ants[k].pheromone = 1 - (distance / 100);
                        angle = atan2(dy, dx) * 180 / M_PI;
                        if (angle < 0)
                        {
                            angle += 360;
                        }
                        ants[k].direction = angle;
                        for (int e = 0; e < antsNum; e++)
                        {
                            if (is_effected_pheromone(ants[e], ants[k]) == -1)
                                continue;
                            else if (ants[e].pheromone != 1 && is_effected_pheromone(ants[e], ants[k]))
                            {
                                printf("----###----Ant #%d is smelling pheromone and changing the direction by 5 degrees!\n", e);
                                dx = foods[i].x - ants[e].x;
                                dy = foods[i].y - ants[e].y;
                                ants[e].pheromone = 0.2;
                                angle = atan2(dy, dx) * 180 / M_PI;
                                if (angle < 0)
                                {
                                    angle += 360;
                                }
                                if (ant[k].direction < angle)
                                {
                                    ants[k].direction += 5;
                                }
                                else
                                {
                                    ants[k].direction -= 5;
                                }
                            }
                        }
                    }
                }
            }
            
            // Change direction towards the food
            dx = foods[i].x - ant->x;
            dy = foods[i].y - ant->y;
            if (foods[i].amount > 0)
            {
                xDist = (ant->x - foods[i].x);
                yDist = (ant->y - foods[i].y);
            }
            angle = atan2(dy, dx) * 180 / M_PI; // atan2(x) returns a value in radians bitween -pi to pi (-180 to 180)
                                                // then we multiply by 180/pi to convert it to degrees.
            if (angle < 0)
            {
                angle += 360;
            }

            ant->direction = angle;
            printf("-----2. The ant is heading toward the food with direction of %d\n", ant->direction);
            // pthread_mutex_unlock(&ant_updating_mutex);
            //  Break out of the loop
            break;
        }
    }

    int j;
    // Check if the ant is on top of food
    for (int i = 0; i < num_food; i++)
    {
        if ((ant->x == foods[i].x && ant->y == foods[i].y) && foods[i].amount > 0)
        {
            printf("------3. The ant reached the piece of food #%d!!!!\n", i);
            pthread_mutex_lock(&ant_eating_mutex);
            // Start eating the food
            ant->eating = 1;

            j = i;

            // Check if the ant is eating
            if (ant->eating)
            {
                // Eat the food
                printf("-----4. The Ant is eating portion #%d!!!!!\n", j);
                sleep(2);
                foods[j].amount -= foodPortionPercent;

                // Check if the food is eaten
                if (foods[j].amount <= 0)
                {
                    // Remove the food

                    foods[j].amount = 0;
                    printf("--##--The food portion #%d has finished!!!!!\n", j);
                }

                // Stop eating
                ant->eating = 0;
                printf("-----5. The ant has finished its portion from the piece of food #%d ^-^\n", j);
                pthread_mutex_unlock(&ant_eating_mutex);
            }
            // Break out of the loop
            break;
        }
    }
    xSpeed = ant->speed; ySpeed = ant->speed;

    if (xDist < (xSpeed * cos(ant->direction)))
    {
        xSpeed = xDist / cos(ant->direction);
        printf("--###--The food is near now! the xSpeed decreased to %d\n", xSpeed);
    }
    if (yDist < (ySpeed * sin(ant->direction)))
    {
        ySpeed = yDist / sin(ant->direction);
        printf("--###--The food is near now! the ySpeed decreased to %d\n", ySpeed);
    }

    ant->x += xSpeed * cos(ant->direction);
    ant->y += ySpeed * sin(ant->direction);

    // Check if the ant hit the edge of the screen
    if (ant->x < 0 || ant->x >= 100 || ant->y < 0 || ant->y >= 100)
    {

        // Reverse the direction
        int d = rand() % 1;
        // Reverse the direction
        if (d == 0)
        { // turn 45 CW
            ant->direction = (ant->direction + 45) % 360;
            printf("---!!---Ant is on the edge, and it turns CW now!\n");
        }
        else
        { // turn 45 CCW
            ant->direction = (ant->direction - 45) % 360;
            printf("---!!---Ant is on the edge, and it turns CCW now!\n");
        }
    }
}

// Function to update the position of all ants
void update_ants()
{

    for (int i = 0; i < antsNum; i++)
    {
        pthread_create(&antsThreads[i],
                       NULL,
                       (void *)update_ant,
                       (void *)&ants[i]);
        pthread_join(antsThreads[i], NULL);
    }

    // for (int i = 0; i < antsNum; i++)
    // {
    //     pthread_join(antsThreads[i], NULL);
    // }
}

void antsCreation()
{

    for (int i = 0; i < antsNum; i++)
    {
        pthread_create(&antsThreads[i],
                       NULL,
                       (void *)init_ant,
                       NULL);
    }

    for (int i = 0; i < antsNum; i++)
    {
        // Join the food thread
        pthread_join(antsThreads[i], NULL);
    }
}

//************************************************** OPENGL *****************************************
// void ShowRange(float radius, float centerX, float centerY){
//     glColor3f(1.0, 1.0, 1.0); // Set border color to white
//     glLineWidth(2.0); // Set border line width

//     glBegin(GL_LINE_LOOP);
//     for (int i = 0; i < numSegments; i++) {
//         float theta = 2.0f * PI * i / numSegments;
//         float x = radius * cosf(theta);
//         float y = radius * sinf(theta);
//         glVertex2f(centerX + x, centerY + y);
//     }
//     glEnd();

//     glFlush();
// }

// void drawFood(float x, float y) {
//     glPushMatrix();
//     glTranslatef(x, y, 0.0f);
//     glColor3f(0.0f, 1.0f, 0.0f);    // Set the color to red
//     glBegin(GL_QUADS);
//     glVertex2f(-10.0f, -10.0f);     // Bottom-left vertex
//     glVertex2f(10.0f, -10.0f);      // Bottom-right vertex
//     glVertex2f(10.0f, 10.0f);       // Top-right vertex
//     glVertex2f(-10.0f, 10.0f);      // Top-left vertex
//     glEnd();
//     glPopMatrix();

// }

// void drawSquare(Ant ant) {
//     glPushMatrix();
//     glTranslatef(x, y, 0.0f);
//     glColor3f(0.4f, 0.2f, 0.0f);  // Dark brown
//     glBegin(GL_QUADS);            // Begin drawing rectangles
//     glVertex2f(-5.0f, -10.0f);     // Bottom-left vertex
//     glVertex2f(5.0f, -10.0f);      // Bottom-right vertex
//     glVertex2f(5.0f, 10.0f);       // Top-right vertex
//     glVertex2f(-5.0f, 10.0f);      // Top-left vertex
//     glEnd();
//     glPopMatrix();
// }

// void removePheromonreRange(){
//     for (int i = 0; i < MAX_SQUARES; i++) {
//     	squares[i].pheromoneLevel = 0.0f;
//     }
// }

// void timer(int value) {
//     update_ant();
//     // Check for food detection
//     //checkFoodDetection();
//     // Check if it's time to place new food
//     int currentTime = glutGet(GLUT_ELAPSED_TIME);
//     if (currentTime - lastFoodPlacementTime >= foodPlacementInterval) {
//         placeFood();
//         lastFoodPlacementTime = currentTime;
//     }
//     removePheromonreRange();
//     // Update ant directions based on food location and pheromone levels
//     updateAntDirections();
//     glutPostRedisplay();            // Mark the current window as needing to be redisplayed
//     glutTimerFunc(16, timer, 0);    // Call the timer function after 16 milliseconds (about 60 FPS)
// }

// void display() {
//     glClear(GL_COLOR_BUFFER_BIT);   // Clear the color buffer
//     glMatrixMode(GL_MODELVIEW);     // Set the current matrix mode
//     glLoadIdentity();               // Load the identity matrix

//     // Draw squares
//     for (int i = 0; i < MAX_SQUARES; i++) {
//     	drawSquare(squares[i].x, squares[i].y, squares[i].pheromoneLevel);
//     	ShowRange(squares[i].pheromoneLevel,squares[i].x, squares[i].y);

//     }
//     for (int i = 0; i < MAX_FOOD; i++) {
//     	drawFood(foods[i].x,foods[i].y);
//     	ShowRange(foodDetectionRadius, foods[i].x,foods[i].y);
//     }
//     glFlush();                      // Flush the rendering pipeline
//     glutSwapBuffers();              // Swap the front and back buffers (if using double buffering)
// }

// void reshape(int width, int height) {
//     glViewport(0, 0, width, height);        // Set the viewport to cover the entire window
//     glMatrixMode(GL_PROJECTION);            // Set the projection matrix mode
//     glLoadIdentity();                       // Load the identity matrix
//     glOrtho(0, width, 0, height, -1, 1);     // Set the orthographic projection
//     glMatrixMode(GL_MODELVIEW);             // Switch back to the modelview matrix mode
//     glLoadIdentity();                       // Load the identity matrix
// }

//  void startOpenGl(){
//     glutInit(&argc, argv);
//     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
//     glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
//     glutCreateWindow("Ants with Pheromones");
//     glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
//     glMatrixMode(GL_MODELVIEW);
//     glLoadIdentity();

//     initSquares();
//     placeFood();
//     lastFoodPlacementTime = glutGet(GLUT_ELAPSED_TIME);

//     glutDisplayFunc(display);
//     glutReshapeFunc(reshape);
//     glutTimerFunc(0, timer, 0);

//     glutMainLoop();
// }

#endif
