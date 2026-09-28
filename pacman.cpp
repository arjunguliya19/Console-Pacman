#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include "colours.h"

#define PACMAN 'P'
#define GHOST 'G'
#define DOT '.'
#define WALL 'W'
#define EMPTY ' '

//Declaration of all functions
void printMap(char **map, int pacInitialX, int pacInitialY, int pacFinalX, int pacFinalY, int loss);
int winCheck(int dotnum);
int loseCheck(int pacFinalY, int pacFinalX, char **map);
int isWall(int pacFinalY, int pacFinalX, char **map);
int dotCounter(int pacFinalY, int pacFinalX, char **map);
int ghostMovement(int ghostInitialX, int ghostInitialY, char **map, int direction, int holdingDot, int ghostNum);
int ghostDirection(char **map, int direction, int ghostNum);
int ghostChase(char **map, int ghostInitialX, int ghostInitialY, int pacInitialX, int pacInitialY, int originalDirection);
void findBigSmall(int num1, int num2);
//Declaration of global variables
int counter = 0;
int totalDots = 0;
int BigSmall[2];
//These two global arrays will store the final and initial coordinates of the ghosts, these will then
//be used in multiple functions
int ghostFinal[2][2] = {{1, 9}, {1, 9}};
int ghostInitial[2][2] = {{1, 9}, {1, 9}};

int main() {
    //Seed the random number generator so the ghosts move differently every game
    srand(time(NULL));

    //First open the map file and store its contents in an array
    FILE *fMap = fopen("map.txt", "r");
    char **map = (char**) malloc(11 * sizeof(char*));
    for(int i = 0; i < 11; i++){
        map[i] = (char*) malloc(11 * sizeof(char));
    }

    //First the outer wall is created, then the inner part is filled using input from the file
    for(int i = 0; i < 11; i++){
        map[0][i] = WALL; map[10][i] = WALL;
        map[i][0] = WALL; map[i][10] = WALL;
    }
    for (int j = 1; j < 10; j++){
        for (int i = 1; i < 10; i++){
            fscanf(fMap, "%c  ", &map[j][i]);
        }
    }//Close the file
    fclose(fMap);

    //Here we start by declaring all the variables, and initialising them with some starting values
    int pacInitialX = 5, pacInitialY = 5;
    int pacFinalX = pacInitialX, pacFinalY = pacInitialY;
    int holdingDot1 = 1, holdingDot2 = 1;
    //The direction of each ghost gets initialised with a random value
    int direction1 = (rand() % 4 +1), direction2 = (rand() % 4 +1);
    direction1 = ghostDirection(map, direction1, 1);
    direction2 = ghostDirection(map, direction2, 2);

    //There was a bug in Clion where the top left ghost was bugged, so we had to fix it by manually inputting it
    map[1][1] = GHOST;

    //Count the dots needed to win. Each ghost starts on a square that becomes a dot when it moves off,
    //so ghosts are counted as well
    for (int j = 1; j < 10; j++){
        for (int i = 1; i < 10; i++){
            if (map[j][i] == DOT || map[j][i] == GHOST){
                totalDots++;
            }
        }
    }

    //Start by printing out an inital map
    printMap(map, pacInitialX, pacInitialY, pacFinalX, pacFinalY, 0);

    while (1){

        //Calling all functions related to moving the ghosts

        //First calling the ghost direction function to get a viable direction to move in
        direction1 = ghostDirection(map, direction1, 1);
        direction2 = ghostDirection(map, direction2, 2);
        //Then call the ghost chase function to see if the ghost can chase pacman or not, and change direction accordingly
        direction1 = ghostChase(map, ghostFinal[0][0], ghostFinal[1][0], pacInitialX, pacInitialY, direction1);
        direction2 = ghostChase(map, ghostFinal[0][1], ghostFinal[1][1], pacInitialX, pacInitialY, direction2);
        //Then finally call the ghost movement function to move the ghost based on the direction that we got from the
        //previous functions
        holdingDot1 = ghostMovement(ghostFinal[0][0], ghostFinal[1][0], map, direction1, holdingDot1, 1);
        holdingDot2 = ghostMovement(ghostFinal[0][1], ghostFinal[1][1], map, direction2, holdingDot2, 2);
        //If a ghost moved onto pacman's square, pacman is caught before he gets to move
        if (map[pacInitialY][pacInitialX] == GHOST){
            printMap(map, pacInitialX, pacInitialY, pacInitialX, pacInitialY, 1);
            printf("Sorry, you lose.\nPress any key to exit the game");
            break;
        }
        //...................................................................................................
        //Get input for pacman's movement, then change the coordinate of pacman based on the user's input
        int xMovement = 0, yMovement = 0;
        switch(getch()){
            case 'w':
                yMovement = -1;
                break;
            case 's':
                yMovement = +1;
                break;
            case 'a':
                xMovement = -1;
                break;
            case 'd':
                xMovement = 1;
                break;
        }
        //Change the final coordinate of pacman
        pacFinalX = pacInitialX + xMovement; pacFinalY = pacInitialY + yMovement;
        //If pacman's next location is a wall, make pacman stay in the initial position
        if (isWall(pacFinalY, pacFinalX, map)) {
            pacFinalX -= xMovement;
            pacFinalY -= yMovement;
        }
        //This function checks if the position pacman is moving to is a dot. If it is then
        // it increases the dotCounter by 1

        dotCounter(pacFinalY,pacFinalX, map);
        //Check if the losing condition is met, if not continue the game
        if (!loseCheck(pacFinalY, pacFinalX, map)){
            printMap(map, pacInitialX, pacInitialY, pacFinalX, pacFinalY, 0);
        }
        else{
            printMap(map, pacInitialX, pacInitialY, pacFinalX, pacFinalY, 1);
            printf("Sorry, you lose.\nPress any key to exit the game");
            break;
        }


        //Check if the winning condition is met by checking if every dot has been eaten
        if (winCheck(dotCounter(pacFinalY, pacFinalX, map))){
            printf("Congratulations! You win!\nPress any key to exit the game");
            break;
        }
        //Update pacman's coordinate for the next loop iteration by making the final coordinates the initial coordinates
        pacInitialX = pacFinalX; pacInitialY = pacFinalY;
//...................................................................................................
    }
    getch();
    return 0;
}
//The printMap function takes in the final and initial coordinates of pacman, to update his movement.
//colourChange function is called inside this function to change the color of the map and the characters.
void printMap(char **map, int pacInitialX, int pacInitialY, int pacFinalX, int pacFinalY, int loss)
{
    //Change the dots on the map into empty space whenever pacman goes over the dots.
    if (loss == 0) {
        map[pacFinalY][pacFinalX] = PACMAN;
        if (map[pacInitialY][pacInitialX] != GHOST) {
            map[pacInitialY][pacInitialX] = EMPTY;
        }
        if (pacFinalY == pacInitialY && pacFinalX == pacInitialX) {
            map[pacInitialY][pacInitialX] = PACMAN;
        }
    }
    else if (map[pacInitialY][pacInitialX] != GHOST){
        map[pacInitialY][pacInitialX] = EMPTY;
    }
    //This portion of the code is to print the whole map with their respective colours.
    for (int j = 0; j < 11; j++)
    {
        for (int i = 0; i < 11; i++)
        {   //If it's a W change text color to blue
            if (map[j][i] == WALL){
                colourChange(BLUE);
            } //If it's a P change text color to yellow
            else if (map[j][i] == PACMAN){
                colourChange(YELLOW);
            } //If it's a G change text color to pink
            else if (map[j][i] == GHOST){
                colourChange(PINK);
            } //If it's a . change text color to white
            else if (map[j][i] == DOT){
                colourChange(WHITE);
            }
            printf("%c  ", map[j][i]);
        }
        printf("\n");
    }

}
//dotCounter function counts the number of the dots pacman eats. This function is used with the winCheck function.
//This function takes in the final coordinates of pacman and the map array
int dotCounter(int pacFinalY, int pacFinalX, char **map)
{   //If pacman goes over a dot, add 1 to the counter
    if (map[pacFinalY][pacFinalX] == DOT){
        counter += 1;
    }
    return counter;
}
//The winCheck function checks if a winning condition is met
//It takes in the dot counter global variable which is also used in the dotCounter function
int winCheck(int dotnum)
{   //If the counter is equal to the total number of dots, then it's a win. If not, continue the game.
    if (dotnum == totalDots){
        return 1;
    }
    else{
        return 0;
    }
}
//isWall function checks if the ghost's and pacman's final coordinates is a wall
int isWall(int pacFinalY, int pacFinalX, char **map){
    //True when the final coordinate has a W. Return 1 which means that the final movement is a wall
    if (map[pacFinalY][pacFinalX] == WALL){
        return 1;
    }
    else{
        return 0;
    }
}
//loseCheck function checks if any losing condition is met.
//Takes in the final coordinates of pacman
int loseCheck(int pacFinalY, int pacFinalX, char **map)
{   //The function returns a 1, whenever pacman's final coordinate is the same as the ghosts' final coordinate.
    //Returning a 1 indicates that the losing condition has been met.
    if(map[pacFinalY][pacFinalX] == GHOST || pacFinalY == ghostInitial[1][0] && pacFinalX == ghostInitial[0][0] || pacFinalX == ghostInitial[0][1] && pacFinalY == ghostInitial[1][1]){
        return 1;
    }
    else{
        return 0;
    }
}

//The ghost direction function takes in a direction, and checks if it is viable (i.e. if the ghost runs into a wall or not)
//If the input direction is viable, it returns the same direction
//If not, then it loops in an infinite loop, taking a random direction each time, until it gets a direction that is viable
//This allows the ghost to only change direction when hitting a wall, and otherwise allows it to move in a straight line
int ghostDirection(char **map, int direction, int ghostNum)
{
    //Declare two variables that represent the hypothetical final coordinates of the ghost.
    //These arent the actual final, just temporary ones used just in this function to check whether the ghost
    //will hit a wall or not
    int ghostFinalY, ghostFinalX;
    while(1)
    {
        //This code works exactly the same as the pac man movement code, it makes a final coordinate using
        //inital coordinates and direction.
        int xMovement = 0, yMovement = 0;
        if(direction == 1){
            yMovement = -1;
        }
        else if (direction == 2){
            yMovement = +1;
        }
        else if (direction == 3){
            xMovement = -1;
        }
        else if (direction == 4){
            xMovement = 1;
        }

        //Two different coordinates for two difference ghosts
        if(ghostNum == 1){
            ghostFinalX = ghostFinal[0][0] + xMovement; ghostFinalY = ghostFinal[1][0] + yMovement;
        }
        else if (ghostNum == 2){
            ghostFinalX = ghostFinal[0][1] + xMovement; ghostFinalY = ghostFinal[1][1] + yMovement;
        }

        //If the ghost doesnt end up hitting a wall, then the loop breaks, returning the direction that allows it to
        //not hit a wall
        if (!isWall(ghostFinalY, ghostFinalX, map)){
            break;
        }
        //If the ghost does hit a wall then a new random direction is used and the loop is repeated
        direction = (rand() % 4 +1);
    }
    return direction;
}

int ghostMovement(int ghostInitialX, int ghostInitialY, char **map, int direction, int holdingDot, int ghostNum)
{
    //Just like the pac man movement code, it takes inital coordinates and a direction as provided by other functions
    //to get a new final coordinate of the ghost that it will move to
    int ghostFinalY, ghostFinalX;
    int xMovement = 0, yMovement = 0;
    if(direction == 1){
        yMovement = -1;
    }
    else if (direction == 2){
        yMovement = +1;
    }
    else if (direction == 3){
        xMovement = -1;
    }
    else if (direction == 4){
        xMovement = 1;
    }

    ghostFinalX = ghostInitialX + xMovement; ghostFinalY = ghostInitialY + yMovement;

    //If the other ghost is in the way, wait this turn instead of moving onto it.
    //Two ghosts on one square would overwrite the dot one of them is holding
    if (map[ghostFinalY][ghostFinalX] == GHOST){
        ghostInitial[0][ghostNum - 1] = ghostInitialX; ghostInitial[1][ghostNum - 1] = ghostInitialY;
        return holdingDot;
    }

    //Define a new variable charge whose purpose is to check if the ghost is going to be moving over a dot
    //hence making the ghost temporarily "Hold the dot"
    // (in this case we thought of it as making the ghost "charged with a dot")
    // until it moves off of that space
    //The value is 1 if it will be holding a dot and 0 if not
    int charge = 0;
    if(map[ghostFinalY][ghostFinalX] == DOT){
        charge = 1;
    }

    map[ghostFinalY][ghostFinalX] = GHOST;

    //holdingDot is the exact same as charge, we just needed to come up with another name for the variable holdingDot
    //as it needed to be used twice, hence we temporarily used "charge"
    //holdingDot check is the ghost in its previous position was charged or not, and depending on the answer either puts a dot
    //or an empty space
    if(holdingDot == 1){
        map[ghostInitialY][ghostInitialX] = DOT;
    }
    else{
        map[ghostInitialY][ghostInitialX] = EMPTY;
    }

    //change the ghosts's initial coordinate to its current coordinate (So that it can be used as initial in the next iteration of the loop)
    //and store the ghosts's initial coordinate in other functions.
    ghostInitial[0][ghostNum - 1] = ghostFinal[0][ghostNum - 1]; ghostInitial[1][ghostNum - 1] = ghostFinal[1][ghostNum - 1];
    ghostFinal[0][ghostNum - 1] = ghostFinalX; ghostFinal[1][ghostNum - 1] = ghostFinalY;

    //change holdingDot value to correspond to the ghosts's current position, and then return it
    holdingDot = charge;
    return holdingDot;
}

//findBigSmall is a function used for the ghostChase function
//It takes 2 coordinates, each for pacman and ghost. The function returns both the smaller and bigger value as an array
//That has been initalised as a global array, and hence it can be accesed by other functions
void findBigSmall(int num1, int num2){
    if (num2 < num1){
        BigSmall[0] = num1;
        BigSmall[1] = num2;
    }
    else if (num1 < num2){
        BigSmall[0] = num2;
        BigSmall[1] = num1;
    }
}
//This function is used for the ghost to chase pacman
//It takes a direction and intial coordinate of ghost and pacman as the arguments
int ghostChase(char **map, int ghostInitialX, int ghostInitialY, int pacInitialX, int pacInitialY, int originalDirection){
    //This portion checks if pacman is in the same coloumn of the ghosts.
    //If pacman is in the same column, the for loop counts if there are any W between the ghost and pacman. If there are W between them, ignore pacman
    int wallCounter = 0;
    if (ghostInitialX == pacInitialX) {
        findBigSmall(ghostInitialY, pacInitialY);
        int small = BigSmall[1];
        int big = BigSmall[0];
        for (int i = small; i <= big; i++) {
            if (map[i][ghostInitialX] == WALL) {
                wallCounter++;
            }
        } //if there are no walls between them, and the ghost is above pacman, return 1 which moves the ghost down.
        //If the ghost is below pacman, return 2 which moves the ghost up.
        if (wallCounter == 0) {
            if (small == pacInitialY) {
                return 1;
            } else {
                return 2;
            }
        } else { //If there are walls, ignore pacman and return original direction
            return originalDirection;
        }
    }
    //This portion checks if pacman is in the same row of the ghosts.
    //If pacman is in the same row, the for loop counts if there are any W between the ghost and pacman. If there are W between them, ignore pacman
    if (ghostInitialY == pacInitialY) {
        findBigSmall(ghostInitialX, pacInitialX);
        int small = BigSmall[1];
        int big = BigSmall[0];
        for (int i = small; i <= big; i++) {
            if (map[ghostInitialY][i] == WALL) {
                wallCounter++;
            }
        }//if there are no walls between them, and the ghost is on the right of pacman, return 3 which moves the ghost to the left.
        //If the ghost is on the left pacman, return 4 which moves the ghost to the right.
        if (wallCounter == 0) {
            if (small == pacInitialX) {
                return 3;
            } else {
                return 4;
            }
        } else {//If there are walls, ignore pacman and return original direction
            return originalDirection;
        }
    }
    return originalDirection;
}