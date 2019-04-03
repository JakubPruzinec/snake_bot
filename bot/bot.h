#pragma once

#include <stdbool.h>

/**
 * Runs snake game (child process function)
 * @param file Path to snake executable
 */
void runSnake(const char *file);

/**
 * Bot process function
 * @param childPid Process ID of child process
 * @param injectTrapsInMemory If set, original snake game is executed and code is modified
 * in memory, otherwise staticaly instrumented snake binary is used
 */
void runBot(pid_t childPid, bool injectTrapsInMemory);
