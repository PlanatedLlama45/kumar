#include <raylib.h>
#include <stdio.h>

#ifndef KUMIR_GRID_H
#define KUMIR_GRID_H


#define nmallocT(T, N) (T *)malloc(N * sizeof(T))
#define mallocT(T) (T *)malloc(sizeof(T))

typedef enum {
    GRID_CELL_NONE,
    GRID_CELL_EMPTY,
    GRID_CELL_FILLED,
    GRID_CELL_WALL
} CellType;

typedef struct Grid {
    int width;
    int height;
    int cellSize;
    Color backgroundColor;
    Color filledBackgroundColor;
    Color wallColor;
    Color gridColor;
    CellType **data;
} Grid;

void generateGridData(Grid *grid) {
    grid->data = nmallocT(CellType *, grid->width);
    for (int x = 0; x < grid->width; x++) {
        grid->data[x] = nmallocT(CellType, grid->height);
        for (int y = 0; y < grid->height; y++)
            grid->data[x][y] = GRID_CELL_EMPTY;
    }
}

void setGridCell(Grid *grid, int x, int y, CellType value) {
    if (x < 0 || x >= grid->width || y < 0 || y >= grid->height) {
        printf("Index out of bounds: x = %d, y = %d\n", x, y);
        return;
    }
    grid->data[x][y] = value;
}

CellType getGridCell(Grid grid, int x, int y) {
    if (x < 0 || x >= grid.width || y < 0 || y >= grid.height)
        return GRID_CELL_NONE;
    return grid.data[x][y];
}

bool isGridCellWall(Grid grid, int x, int y) {
    CellType cell = getGridCell(grid, x, y);
    return cell == GRID_CELL_WALL || cell == GRID_CELL_NONE;
}

#define FILENAME_MAX_LENGTH 128
#define GRID_EXTENSION "kum_grid"

bool streq(const char *str1, const char *str2) {
    return !strcmp(str1, str2);
}

char *getFileExt(char *filename) {
    char *ext = nmallocT(char, FILENAME_MAX_LENGTH);
    size_t i, j;
    for (i = 0; filename[i] != '.' && i < strlen(filename); i++);
    for (j = ++i; j < strlen(filename); j++)
        ext[j - i] = filename[j];
    ext[j - i] = '\0';
    return ext;
}

int loadGridFromFile(Grid *grid, const char *filename, int *robotPosX, int *robotPosY) {
    char *m_filename = strdup(filename);
    char *fileExt = getFileExt(m_filename);
    if (fileExt[0] == '\0' || !streq(fileExt, GRID_EXTENSION)) {
        printf("Incorrect file extension. Expected \"*." GRID_EXTENSION "\"");
        return EXIT_FAILURE;
    }
    free(fileExt);

    if (!FileExists(m_filename)) {
        puts("File doesn't exist (or doesn't have an extension)");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        puts("Failed to open file");
        return EXIT_FAILURE;
    }

    generateGridData(grid);

    fread(robotPosX, sizeof(int), 1, file);
    fread(robotPosY, sizeof(int), 1, file);

    int x, y;
    CellType type;
    while (!feof(file)) {
        fread(&type, sizeof(CellType), 1, file);
        fread(&x, sizeof(int), 1, file);
        fread(&y, sizeof(int), 1, file);
        
        setGridCell(grid, x, y, type);
    }

    fclose(file);
    return EXIT_SUCCESS;
}

void flipGridColor(Grid *grid, int x, int y) {
    if (grid->data[x][y] == GRID_CELL_EMPTY)
        grid->data[x][y] = GRID_CELL_FILLED;
    else if (grid->data[x][y] == GRID_CELL_FILLED)
        grid->data[x][y] = GRID_CELL_EMPTY;
}

void drawGrid(Grid grid, int screenWidth, int screenHeight) {
    int xMin = (screenWidth - grid.width * grid.cellSize) / 2;
    int yMin = (screenHeight - grid.height * grid.cellSize) / 2;

    int xMax = (screenWidth + grid.width * grid.cellSize) / 2;
    int yMax = (screenHeight + grid.height * grid.cellSize) / 2;

    Color cellColor;
    for (int x = 0, xPos = xMin; x < grid.width; x++, xPos+=grid.cellSize) {
        for (int y = 0, yPos = yMin; y < grid.height; y++, yPos+=grid.cellSize) {
            if (grid.data[x][y] == GRID_CELL_EMPTY)
                cellColor = grid.backgroundColor;
            else if (grid.data[x][y] == GRID_CELL_FILLED)
                cellColor = grid.filledBackgroundColor;
            else if (grid.data[x][y] == GRID_CELL_WALL)
                cellColor = grid.wallColor;
            DrawRectangle(xPos, yPos, grid.cellSize, grid.cellSize, cellColor);
        }
    }
    
    for (int x = xMin; x <= xMax; x+=grid.cellSize)
        DrawLineEx((Vector2){ x, yMin }, (Vector2){ x, yMax }, 2, grid.gridColor);
    for (int y = yMin; y <= yMax; y+=grid.cellSize)
        DrawLineEx((Vector2){ xMin, y }, (Vector2){ xMax, y }, 2, grid.gridColor);
}

void freeGrid(Grid *grid) {
    for (size_t x = 0; x < grid->width; x++)
        free(grid->data[x]);
    free(grid->data);
}

void getGridMousePos(Grid grid, int *x, int *y, int screenWidth, int screenHeight) {
    Vector2 mousePos = GetMousePosition();

    int xMin = (screenWidth - grid.width * grid.cellSize) / 2;
    int yMin = (screenHeight - grid.height * grid.cellSize) / 2;

    mousePos.x -= xMin;
    mousePos.y -= yMin;

    mousePos.x /= grid.cellSize;
    mousePos.y /= grid.cellSize;

    int m_x = (int)mousePos.x;
    int m_y = (int)mousePos.y;

    if (0 <= m_x && m_x < grid.width &&
        0 <= m_y && m_y < grid.height) {
        *x = m_x;
        *y = m_y;
    } else {
        *x = -1;
        *y = -1;
    }
}

void dumpGrid(Grid grid, const char *filename, int robotPosX, int robotPosY) {
    FILE *file = fopen(filename, "wb");

    fwrite(&robotPosX, sizeof(int), 1, file);
    fwrite(&robotPosY, sizeof(int), 1, file);

    for (int x = 0; x < grid.width; x++) {
        for (int y = 0; y < grid.height; y++) {
            if (grid.data[x][y] != GRID_CELL_EMPTY && grid.data[x][y] != GRID_CELL_NONE) {
                fwrite(&grid.data[x][y], sizeof(CellType), 1, file);
                fwrite(&x, sizeof(int), 1, file);
                fwrite(&y, sizeof(int), 1, file);
            }
        }
    }

    fclose(file);
}


#endif // !KUMIR_GRID_H
