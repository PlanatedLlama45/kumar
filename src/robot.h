#include "grid.h"

#ifndef KUMIR_ROBOT_H
#define KUMIR_ROBOT_H


typedef struct Robot {
    int posX;
    int posY;
    int size;
    Color color;
    int innerSize;
    Color innerColor;
} Robot;

void drawRobot(Robot robot, Grid grid, int screenWidth, int screenHeight) {
    int posX = (screenWidth - grid.width * grid.cellSize) / 2 + (robot.posX + 0.5f) * grid.cellSize;
    int posY = (screenHeight - grid.height * grid.cellSize) / 2 + (robot.posY + 0.5f) * grid.cellSize;

    DrawCircle(posX, posY, robot.size, robot.color);
    DrawCircle(posX, posY, robot.innerSize, robot.innerColor);
}

int robotGoUp(Robot *robot, Grid grid) {
    if (grid.data[robot->posX][robot->posY - 1])
        return EXIT_FAILURE;
    robot->posY--;
    return EXIT_SUCCESS;
}
int robotGoDown(Robot *robot, Grid grid) {
    if (grid.data[robot->posX][robot->posY + 1])
        return EXIT_FAILURE;
    robot->posY++;
    return EXIT_SUCCESS;
}
int robotGoLeft(Robot *robot, Grid grid) {
    if (grid.data[robot->posX - 1][robot->posY])
        return EXIT_FAILURE;
    robot->posX--;
    return EXIT_SUCCESS;
}

int robotGoRight(Robot *robot, Grid grid) {
    if (grid.data[robot->posX + 1][robot->posY])
        return EXIT_FAILURE;
    robot->posX++;
    return EXIT_SUCCESS;
}

int robotSetPos(Robot *robot, Grid grid, int x, int y) {
    if (isGridCellWall(grid, x, y))
        return EXIT_FAILURE;
    robot->posX = x;
    robot->posY = y;
    return EXIT_SUCCESS;
}


#endif // !KUMIR_ROBOT_H
