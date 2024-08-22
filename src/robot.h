#include "grid.h"

#ifndef KUMIR_ROBOT_H
#define KUMIR_ROBOT_H


typedef struct Robot {
    int posX;
    int posY;
    float size;
    Color color;
    float innerSize;
    Color innerColor;
} Robot;

void drawRobot(Robot robot, Grid grid, int screenWidth, int screenHeight) {
    int posX = (screenWidth - grid.width * grid.cellSize) / 2 + (robot.posX + 0.5f) * grid.cellSize;
    int posY = (screenHeight - grid.height * grid.cellSize) / 2 + (robot.posY + 0.5f) * grid.cellSize;

    DrawCircle(posX, posY, robot.size, robot.color);
    DrawCircle(posX, posY, robot.innerSize, robot.innerColor);
}

int robotGoUp(Robot *robot, Grid grid) {
    if (isGridCellWall(grid, robot->posX, robot->posY - 1))
        return EXIT_FAILURE;
    robot->posY--;
    return EXIT_SUCCESS;
}
int robotGoDown(Robot *robot, Grid grid) {
    if (isGridCellWall(grid, robot->posX, robot->posY + 1))
        return EXIT_FAILURE;
    robot->posY++;
    return EXIT_SUCCESS;
}
int robotGoLeft(Robot *robot, Grid grid) {
    if (isGridCellWall(grid, robot->posX - 1, robot->posY))
        return EXIT_FAILURE;
    robot->posX--;
    return EXIT_SUCCESS;
}

int robotGoRight(Robot *robot, Grid grid) {
    if (isGridCellWall(grid, robot->posX + 1, robot->posY))
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

typedef enum {
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} Direction;

bool robotCheckWall(Robot robot, Grid grid, Direction dir) {
    if (dir == DIRECTION_UP) return isGridCellWall(grid, robot.posX, robot.posY - 1);
    if (dir == DIRECTION_DOWN) return isGridCellWall(grid, robot.posX, robot.posY + 1);
    if (dir == DIRECTION_LEFT) return isGridCellWall(grid, robot.posX - 1, robot.posY);
    if (dir == DIRECTION_RIGHT) return isGridCellWall(grid, robot.posX + 1, robot.posY);
    return false;
}


#endif // !KUMIR_ROBOT_H
