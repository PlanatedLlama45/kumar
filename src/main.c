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

#define SECONDS_PER_LINE_CYCLE 0.1

int runProgram(int argc, const char **argv) {
    if (argc == 1) {
        puts("No filename found");
        return -1;
    } else if (argc == 2) {
        puts("No grid data filename found");
        return -1;
    }
    FILE *file = openFile(argv[1]);
// #ifdef LANG_RU
//     FILE *file = openFile("test");
// #endif // LANG_RU

// #ifdef LANG_EN
//     FILE *file = openFile("test2");
// #endif // LANG_EN
    if (file == NULL) return EXIT_FAILURE;

    Grid grid = (Grid){
        .width = 15,
        .height = 15,
        .cellSize = 50,
        .backgroundColor = GREEN,
        .filledBackgroundColor = PURPLE,
        .wallColor = GRAY,
        .gridColor = YELLOW,
        .data = NULL
    };
    if (loadGridFromFile(&grid, argv[2]) == EXIT_FAILURE) return EXIT_FAILURE;

    Robot robot = (Robot){
        .posX = 7,
        .posY = 7,
        .size = 15,
        .color = GRAY,
        .innerSize = 10,
        .innerColor = LIGHTGRAY
    };

    initInterpreter(&robot, &grid);

    size_t currentLine = 0;
    float secondsSinceLineCycle = 0;
    bool interpreterRunning = true;
    InterpreterExitCode interpreterCode;
    bool skipNextLineDelay = false;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Kumar");

    while (!WindowShouldClose()) {
        if (interpreterRunning) {
            if (!skipNextLineDelay)
                secondsSinceLineCycle += GetFrameTime();
            if (skipNextLineDelay || secondsSinceLineCycle >= SECONDS_PER_LINE_CYCLE) {
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

    CloseWindow();

    fclose(file);
    return EXIT_SUCCESS;
}

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

    Grid grid = (Grid){
        .width = 15,
        .height = 15,
        .cellSize = 50,
        .backgroundColor = GREEN,
        .filledBackgroundColor = PURPLE,
        .wallColor = GRAY,
        .gridColor = YELLOW,
        .data = NULL
    };

    generateGridData(&grid);

    int selectedX, selectedY;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Kumar (grid editor)");

    while (!WindowShouldClose()) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            getGridMousePos(grid, &selectedX, &selectedY, SCREEN_WIDTH, SCREEN_HEIGHT);
            if (selectedX != -1)
                setGridCell(&grid, selectedX, selectedY, CELL_WALL);
        } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            getGridMousePos(grid, &selectedX, &selectedY, SCREEN_WIDTH, SCREEN_HEIGHT);
            if (selectedX != -1)
                setGridCell(&grid, selectedX, selectedY, CELL_FILLED);
        }

        BeginDrawing();
            ClearBackground(BLACK);
            drawGrid(grid, SCREEN_WIDTH, SCREEN_HEIGHT);
        EndDrawing();
    }

    CloseWindow();
    dumpGrid(grid, filename);

    return EXIT_SUCCESS;
}

int main(int argc, const char **argv) {
    if (argc == 1) {
        puts("Not enough arguments");
        return EXIT_FAILURE;
    }
    if (streq(argv[1], "-run"))
        return runProgram(argc - 1, argv + 1);
    if (streq(argv[1], "-grid"))
        return runGridEditor(argc - 1, argv + 1);
    
    puts("Unexpected token at position 1");
    return EXIT_FAILURE;
}
