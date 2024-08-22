#include <raylib.h>

#include "interpreter.h"
#include "robot.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define FILE_EXTENSION "kum"

FILE *openFile(const char *filename) {
    char *m_filename = strdup(filename);
    char *fileExt = getFileExt(m_filename);
    if (fileExt[0] == '\0' || !streq(fileExt, FILE_EXTENSION)) {
        puts("Incorrect file extension. Expected \"*." FILE_EXTENSION "\"");
        return NULL;
    }
    free(fileExt);

    if (!FileExists(m_filename)) {
        puts("File doesn't exist (or doesn't have an extension)");
        return NULL;
    }
    FILE *file = fopen(m_filename, "r");
    if (file == NULL) {
        puts("Failed to open file");
        return NULL;
    }
    return file;
}

Grid makeGrid() {
    return (Grid){
        .width = 15,
        .height = 15,
        .cellSize = 50,
        .backgroundColor = GREEN,
        .filledBackgroundColor = PURPLE,
        .wallColor = GRAY,
        .gridColor = YELLOW,
        .data = NULL
    };
}

Robot makeRobot() {
    return (Robot){
        .posX = 0,
        .posY = 0,
        .size = 15,
        .color = GRAY,
        .innerSize = 10,
        .innerColor = LIGHTGRAY
    };
}

#define SECONDS_PER_STEP_DEFAULT 0.05

int runProgram(int argc, const char **argv) {
    if (argc == 1) {
        puts("No filename found");
        return -1;
    } else if (argc == 2) {
        puts("No grid data filename found");
        return -1;
    }
    FILE *file = openFile(argv[1]);
    if (file == NULL) return EXIT_FAILURE;

    Grid grid = makeGrid();
    Robot robot = makeRobot();

    if (loadGridFromFile(&grid, argv[2], &robot.posX, &robot.posY) == EXIT_FAILURE) return EXIT_FAILURE;

    initInterpreter(&robot, &grid);

    float secondsPerLineCycle;
    bool isInstant = false;
    if (argc == 3)
        secondsPerLineCycle = SECONDS_PER_STEP_DEFAULT;
    else {
        secondsPerLineCycle = atof(argv[3]);
        if (secondsPerLineCycle > 0.f)
            secondsPerLineCycle = 1.f / secondsPerLineCycle;
        else
            isInstant = true;
    }

    size_t currentLine = 0;
    float secondsSinceLineCycle = 0;
    bool interpreterRunning = true;
    InterpreterExitCode interpreterCode;
    bool skipNextLineDelay = false;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Kumar");

    while (!WindowShouldClose()) {
        if (interpreterRunning) {
            if (!isInstant && !skipNextLineDelay)
                secondsSinceLineCycle += GetFrameTime();
            if (isInstant || skipNextLineDelay || secondsSinceLineCycle >= secondsPerLineCycle) {
                skipNextLineDelay = false;
                // printf(" -=- Line: %llu -=-\n", currentLine + 1);
                interpreterCode = interpretLine(file, &currentLine);
                if (interpreterCode != INTERPRETER_NORMAL && interpreterCode != INTERPRETER_SKIP_LINE) {
                    interpreterRunning = false;
                    printErrcode(interpreterCode, currentLine + 1);
                    freeInterpreter();
                } else
                    currentLine++;
                if (interpreterCode == INTERPRETER_SKIP_LINE)
                    skipNextLineDelay = true;
                secondsSinceLineCycle = 0;
            }
        }

        BeginDrawing();
            ClearBackground(BLACK);
            drawGrid(grid, SCREEN_WIDTH, SCREEN_HEIGHT);
            drawRobot(robot, grid, SCREEN_WIDTH, SCREEN_HEIGHT);
        EndDrawing();
    }
    freeInterpreter();

    CloseWindow();

    fclose(file);
    return EXIT_SUCCESS;
}

#define ROBOT_HOLD_SCALE_FACTOR 1.2f
#define ROBOT_HOLD_ALPHA 200

int runGridEditor(int argc, const char **argv) {
    if (argc == 1) {
        puts("No filename found");
        return EXIT_FAILURE;
    }
    char *filename = strdup(argv[1]);
    char *fileExt = getFileExt(filename);
    if (fileExt[0] == '\0' || !streq(fileExt, GRID_EXTENSION)) {
        puts("Incorrect file extension. Expected \"*." GRID_EXTENSION "\"");
        return EXIT_FAILURE;
    }

    Grid grid = makeGrid();
    Robot robot = makeRobot();

    if (FileExists(filename))
        loadGridFromFile(&grid, filename, &robot.posX, &robot.posY);
    else
        generateGridData(&grid);

    int selectedX, selectedY;
    bool holdingRobot = false;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Kumar (grid editor)");

    while (!WindowShouldClose()) {
        getGridMousePos(grid, &selectedX, &selectedY, SCREEN_WIDTH, SCREEN_HEIGHT);

        if (selectedX != robot.posX || selectedY != robot.posY) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (selectedX != -1) {
                    if (getGridCell(grid, selectedX, selectedY) != GRID_CELL_WALL)
                        setGridCell(&grid, selectedX, selectedY, GRID_CELL_WALL);
                    else
                        setGridCell(&grid, selectedX, selectedY, GRID_CELL_EMPTY);
                }
            } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                if (selectedX != -1) {
                    if (getGridCell(grid, selectedX, selectedY) != GRID_CELL_FILLED)
                        setGridCell(&grid, selectedX, selectedY, GRID_CELL_FILLED);
                    else
                        setGridCell(&grid, selectedX, selectedY, GRID_CELL_EMPTY);
                }
            }
        } else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (!holdingRobot) {
                robot.color.a = ROBOT_HOLD_ALPHA;
                robot.innerColor.a = ROBOT_HOLD_ALPHA;
                robot.innerSize *= ROBOT_HOLD_SCALE_FACTOR;
                robot.size *= ROBOT_HOLD_SCALE_FACTOR;
            }
            holdingRobot = true;
        } if (holdingRobot) {
            robotSetPos(&robot, grid, selectedX, selectedY);
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                holdingRobot = false;
                robot.color.a = 255;
                robot.innerColor.a = 255;
                robot.innerSize /= ROBOT_HOLD_SCALE_FACTOR;
                robot.size /= ROBOT_HOLD_SCALE_FACTOR;
            }
        }

        BeginDrawing();
            ClearBackground(BLACK);
            drawGrid(grid, SCREEN_WIDTH, SCREEN_HEIGHT);
            drawRobot(robot, grid, SCREEN_WIDTH, SCREEN_HEIGHT);
        EndDrawing();
    }

    CloseWindow();
    dumpGrid(grid, filename, robot.posX, robot.posY);

    return EXIT_SUCCESS;
}

/*
 * 
 * Синтаксис:
 *   Изменить поле:     kumar grid <файл поля>
 *   Запустить файл:    kumar run <файл> <файл поля> [шагов в секунду > 0 (если <= 0, то мгновенно), если не указано, то 20 шагов в секунду]
 * 
*/

int main(int argc, const char **argv) {
    if (argc == 1) {
        puts("Not enough arguments");
        return EXIT_FAILURE;
    }
    if (streq(argv[1], "run")) {
        if (runProgram(argc - 1, argv + 1) == EXIT_FAILURE) {
            puts("Unexpected error");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    if (streq(argv[1], "grid")) {
        if (runGridEditor(argc - 1, argv + 1) == EXIT_FAILURE) {
            puts("Unexpected error");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    puts("Unexpected token at position 1");
    return EXIT_FAILURE;
}
