#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "keywords.h"
#include "robot.h"

#ifndef KUMIR_INTERPRETER_H
#define KUMIR_INTERPRETER_H


void _m_strRemoveEOL(char *str) {
    int len = strlen(str);
    if (str[len - 1] == '\n')
        str[len - 1] = '\0';
}

#define MAX_LINE_LENGTH 128
#define MAX_FILE_LINES 128

#define INDENTATION_SPACES 2

typedef enum {
    INTERPRETER_NORMAL,
    INTERPRETER_SKIP_LINE,
    INTERPRETER_FINISHED,
    INTERPRETER_FORCE_EXIT,
    INTERPRETER_ERROR,
    INTERPRETER_INVALID_TOKEN,
    INTERPRETER_STACK_OVERFLOW,
    INTERPRETER_SYNTAX_ERROR
} InterpreterExitCode;

void printErrcode(InterpreterExitCode code, size_t lineNum) {
    if (code == INTERPRETER_NORMAL || code == INTERPRETER_SKIP_LINE) return;
    printf("\033[31mInterpreter error at line %llu: ", lineNum);
    switch (code) {
    case INTERPRETER_ERROR: puts("Unexpected error"); break;
    case INTERPRETER_FINISHED: puts("Program finished"); break;
    case INTERPRETER_FORCE_EXIT: puts("'exit' called to terminate"); break;
    case INTERPRETER_INVALID_TOKEN: puts("Encountered an invalid token"); break;
    case INTERPRETER_STACK_OVERFLOW: puts("Loop stack overflow"); break;
    case INTERPRETER_SYNTAX_ERROR: puts("Syntax error"); break;
    default: break;
    }
    fputs("\033[0m", stdout);
}

InterpreterExitCode _m_fromStdExitCode(int code) {
    if (code == EXIT_SUCCESS)
        return INTERPRETER_NORMAL;
    return INTERPRETER_ERROR;
}

Robot *m_interpreterRobot = NULL;
Grid *m_interpreterGrid = NULL;

typedef enum {
    LOGIC_AND,
    LOGIC_OR,
    // LOGIC_NOT,
    LOGIC_CHECK_UP,
    LOGIC_CHECK_DOWN,
    LOGIC_CHECK_LEFT,
    LOGIC_CHECK_RIGHT,

    LOGIC_NOT_CHECK_UP,
    LOGIC_NOT_CHECK_DOWN,
    LOGIC_NOT_CHECK_LEFT,
    LOGIC_NOT_CHECK_RIGHT
} LogicNodeType;

typedef struct {
    LogicNodeType type;
    void *val1;
    void *val2;
} LogicNode;

bool _m_solveLogicTree(LogicNode *tree) {
    Robot *robot = m_interpreterRobot;
    Grid *grid = m_interpreterGrid;
    if (tree->type == LOGIC_CHECK_UP)
        return !isGridCellWall(*grid, robot->posX, robot->posY - 1);
    else if (tree->type == LOGIC_CHECK_DOWN)
        return !isGridCellWall(*grid, robot->posX, robot->posY + 1);
    else if (tree->type == LOGIC_CHECK_LEFT)
        return !isGridCellWall(*grid, robot->posX - 1, robot->posY);
    else if (tree->type == LOGIC_CHECK_RIGHT)
        return !isGridCellWall(*grid, robot->posX + 1, robot->posY);

    else if (tree->type == LOGIC_NOT_CHECK_UP)
        return isGridCellWall(*grid, robot->posX, robot->posY - 1);
    else if (tree->type == LOGIC_NOT_CHECK_DOWN)
        return isGridCellWall(*grid, robot->posX, robot->posY + 1);
    else if (tree->type == LOGIC_NOT_CHECK_LEFT)
        return isGridCellWall(*grid, robot->posX - 1, robot->posY);
    else if (tree->type == LOGIC_NOT_CHECK_RIGHT)
        return isGridCellWall(*grid, robot->posX + 1, robot->posY);
    
    else if (tree->type = LOGIC_AND)
        return _m_solveLogicTree(tree->val1) && _m_solveLogicTree(tree->val2);
    else if (tree->type = LOGIC_OR)
        return _m_solveLogicTree(tree->val1) || _m_solveLogicTree(tree->val2);
}

void _m_freeLogicTree(LogicNode *tree) {
    if (tree->val1 != NULL)
        _m_freeLogicTree(tree->val1);
    if (tree->val2 != NULL)
        _m_freeLogicTree(tree->val2);
    free(tree);
}

typedef struct LoopStackItem {
    ptrdiff_t offset;
    size_t line;
} LoopStackItem;

#define MAX_STACK_SIZE 32

LoopStackItem *loopStack = NULL;
size_t loopStackSize = 0;

void initInterpreter(Robot *robot, Grid *grid) {
    loopStack = nmallocT(LoopStackItem, MAX_STACK_SIZE);
    loopStackSize = 0;
    m_interpreterRobot = robot;
    m_interpreterGrid = grid;
}

void freeInterpreter() {
    free(loopStack);
}

void nullifyStr(char *str) {
    size_t initLen = strlen(str);
    for (size_t i = 0; i < initLen; i++)
        str[i] = '\0';
}

void _m_endLogicExpressionToken(size_t *i, size_t *exprBegin, char *token) {
    (*i)++;
    *exprBegin = *i + 1;
    nullifyStr(token);
}

#define _m_addKwdCheckToParser(keyword) else if (streq(token, KEYWORD_##keyword##_FULL)) { \
        _m_endLogicExpressionToken(&i, &exprBegin, token); \
        if (logicTree->type == -1) \
            logicTree->type = LOGIC_##keyword; \
        else { \
            LogicNode *tmpNode = mallocT(LogicNode); \
            tmpNode->type = LOGIC_##keyword; \
            tmpNode->val1 = NULL; \
            tmpNode->val2 = NULL; \
            logicTree->val2 = tmpNode; \
        } \
    }

LogicNode *_m_parseLogicExpression(char *line, size_t exprBegin, InterpreterExitCode *exitCode) {
    *exitCode = INTERPRETER_NORMAL;
    char token[MAX_LINE_LENGTH] = { '\0' };

    LogicNode *logicTree = mallocT(LogicNode);
    logicTree->type = -1;
    logicTree->val1 = NULL;
    logicTree->val2 = NULL;

    for (size_t i = exprBegin; i < strlen(line); i++) {
        token[i - exprBegin] = line[i];

        if (streq(token, KEYWORD_THEN)) {
            if (logicTree->type != -1)
                return logicTree;
            *exitCode = INTERPRETER_SYNTAX_ERROR;
        }
        _m_addKwdCheckToParser(CHECK_UP)
        _m_addKwdCheckToParser(CHECK_DOWN)
        _m_addKwdCheckToParser(CHECK_LEFT)
        _m_addKwdCheckToParser(CHECK_RIGHT)

        _m_addKwdCheckToParser(NOT_CHECK_UP)
        _m_addKwdCheckToParser(NOT_CHECK_DOWN)
        _m_addKwdCheckToParser(NOT_CHECK_LEFT)
        _m_addKwdCheckToParser(NOT_CHECK_RIGHT)
        
        else if (streq(token, KEYWORD_AND)) {
            _m_endLogicExpressionToken(&i, &exprBegin, token);

            LogicNode *tmpNode = mallocT(LogicNode);
            tmpNode->type = logicTree->type;
            tmpNode->val1 = logicTree->val1;
            tmpNode->val2 = logicTree->val2;

            logicTree->type = LOGIC_AND;
            logicTree->val1 = tmpNode;
        } else if (streq(token, KEYWORD_OR)) {
            _m_endLogicExpressionToken(&i, &exprBegin, token);

            LogicNode *tmpNode = mallocT(LogicNode);
            tmpNode->type = logicTree->type;
            tmpNode->val1 = logicTree->val1;
            tmpNode->val2 = logicTree->val2;

            logicTree->type = LOGIC_OR;
            logicTree->val1 = tmpNode;
        }
    }
    *exitCode = INTERPRETER_ERROR;
    return logicTree;
}

size_t _m_getTabEndPos(char *line) {
    for (size_t i = 0; i < strlen(line); i++) {
        if (line[i] != ' ')
            return i;
    }
    return SIZE_MAX;
}

InterpreterExitCode _m_setRobotPos(char *line, size_t exprBegin) {
    Robot *robot = m_interpreterRobot;
    Grid *grid = m_interpreterGrid;

    char xStr[11] = { '\0' }, yStr[11] = { '\0' };
    size_t i, j;

    for (i = exprBegin; line[i] != ','; i++)
        xStr[i - exprBegin] = line[i];
    i++;
    while (line[i] == ' ') i++;
    for (j = i; j < strlen(line); j++)
        yStr[j - i] = line[j];
    
    return _m_fromStdExitCode(robotSetPos(robot, *grid, atoi(xStr), atoi(yStr)));
}

InterpreterExitCode interpretLine(FILE *file, size_t *lineNum) {
    Robot *robot = m_interpreterRobot;
    Grid *grid = m_interpreterGrid;

    if (feof(file))
        return INTERPRETER_FINISHED;
    char line[MAX_LINE_LENGTH];

    ptrdiff_t fileOffset = ftell(file);
    fgets(line, MAX_LINE_LENGTH, file);
    if (feof(file)) return INTERPRETER_FINISHED;
    
    size_t indentation = _m_getTabEndPos(line);
    _m_strRemoveEOL(line);
    if (line[indentation] == '\0') return INTERPRETER_SKIP_LINE;

    char token[MAX_LINE_LENGTH] = { '\0' };
    static size_t syntaxDepth = 0;
    static bool lookingForExit = false;
    static size_t exitSyntaxDepth = 0;
    for (size_t i = indentation; i < strlen(line); i++) {
        token[i - indentation] = line[i];

        if (streq(token, KEYWORD_ENDIF)) {
            if (syntaxDepth == 0)
                return INTERPRETER_SYNTAX_ERROR;
            syntaxDepth--;
            if (lookingForExit && syntaxDepth == exitSyntaxDepth)
                lookingForExit = false;
            return INTERPRETER_SKIP_LINE;
        }
        else if (streq(token, KEYWORD_ENDLOOP)) {
            if (loopStackSize == 0)
                return INTERPRETER_SYNTAX_ERROR;
            if (lookingForExit && syntaxDepth == exitSyntaxDepth) {
                lookingForExit = false;
                loopStackSize--;
            } else {
                *lineNum = loopStack[loopStackSize - 1].line - 1;
                fseek(file, loopStack[loopStackSize - 1].offset, SEEK_SET);
            }
            syntaxDepth--;
            return INTERPRETER_SKIP_LINE;
        }

        if (lookingForExit)
            continue;

        if (streq(token, KEYWORD_EXIT))
            return INTERPRETER_FORCE_EXIT;
        else if (streq(token, KEYWORD_IF)) {
            InterpreterExitCode code;
            LogicNode *logicTree = _m_parseLogicExpression(line, i + 2, &code);
            bool result = _m_solveLogicTree(logicTree);
            _m_freeLogicTree(logicTree);
            if (code != INTERPRETER_NORMAL) return code;

            if (result == false) {
                lookingForExit = true;
                exitSyntaxDepth = syntaxDepth;
            }
            syntaxDepth++;
            return INTERPRETER_NORMAL;
        }
        else if (streq(token, KEYWORD_LOOP)) {
            syntaxDepth++;
            if (loopStackSize == MAX_STACK_SIZE)
                return INTERPRETER_STACK_OVERFLOW;
            InterpreterExitCode code;
            LogicNode *logicTree = _m_parseLogicExpression(line, i + 2, &code);
            bool result = _m_solveLogicTree(logicTree);
            _m_freeLogicTree(logicTree);
            loopStack[loopStackSize++] = (LoopStackItem){ .offset = fileOffset, .line = *lineNum };
            if (result == false) {
                lookingForExit = true;
                exitSyntaxDepth = syntaxDepth;
            }
            return INTERPRETER_NORMAL;
        }
        else if (streq(token, KEYWORD_PAINT)) {
            flipGridColor(grid, robot->posX, robot->posY);
            return INTERPRETER_NORMAL;

        } else if (streq(token, KEYWORD_GO_UP))
            return _m_fromStdExitCode(robotGoUp(robot, *grid));

        else if (streq(token, KEYWORD_GO_DOWN))
            return _m_fromStdExitCode(robotGoDown(robot, *grid));

        else if (streq(token, KEYWORD_GO_LEFT))
            return _m_fromStdExitCode(robotGoLeft(robot, *grid));

        else if (streq(token, KEYWORD_GO_RIGHT))
            return _m_fromStdExitCode(robotGoRight(robot, *grid));
        
        else if (streq(token, KEYWORD_SETPOS))
            return _m_setRobotPos(line, i + 2);
    }
    if (lookingForExit)
        return INTERPRETER_SKIP_LINE;
    return INTERPRETER_INVALID_TOKEN;
}


#endif // !KUMIR_INTERPRETER_H
